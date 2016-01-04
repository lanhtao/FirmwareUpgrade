#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <string>
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/StreamCopier.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/MD5Engine.h"
#include "Poco/DigestStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/Util/Application.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/XMLConfiguration.h"

using Poco::Util::Application;
using Poco::DigestEngine;
using Poco::MD5Engine;
using Poco::DigestOutputStream;
using Poco::StreamCopier;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPClientSession;
using Poco::StreamCopier;
using Poco::Net::HTTPResponse;
using Poco::Util::ServerApplication;
using Poco::Exception;
using Poco::AutoPtr;
using Poco::Util::XMLConfiguration;


/**
 * 计算文件的md5值
 */
static std::string  calculate_md5_value(const char* fileName)
{
	std::ifstream istr(fileName, std::ios::binary);
	if (!istr)
	{
		Application::instance().logger().error("open fileName failed");
	}

	MD5Engine md5;
	DigestOutputStream dos(md5);

	StreamCopier::copyStream(istr, dos);
	dos.close();

	return DigestEngine::digestToHex(md5.digest());
}

/*
 * 发送请求，并接收响应。参数url为请求的目录
 */
std::string send_request(HTTPClientSession &s ,std::string url)
{
	HTTPRequest request(HTTPRequest::HTTP_GET, url);
	HTTPResponse response;
	s.sendRequest(request);
	std::istream& rs = s.receiveResponse(response);
	std::string str_response;
	char c;
	while (rs.get(c))
	{
		str_response.append(1,c);
	}
	return str_response;
}

class HttpAplication:public ServerApplication
{
public:
	HttpAplication()
	{
	}

	~HttpAplication()
	{
	}
protected:
	void initialize(Application& self)
	{
		loadConfiguration(); // load default configuration files, if present
		ServerApplication::initialize(self);
	}

	void uninitialize()
	{
		ServerApplication::uninitialize();
	}

	int main(const std::vector<std::string>& args)
	{
		HTTPClientSession s("192.168.1.1",8886);
		int statu = 0;

		try
		{
			AutoPtr<XMLConfiguration> pConf(new XMLConfiguration("HTTPClient.xml"));
			std::string current_version  = pConf->getString("Firmware.version[@value]","1.0.0");
			std::string file_name = pConf->getString("Firmware.filename[@value]","HomeControlCenter_base1.1.0");
			std::string file_download_path = pConf->getString("Firmware.downloadpath[@value]","/tmp/");
			std::string file_storage_path = pConf->getString("Firmware.storagepath[@value]","/root/");
			std::string new_version ;
			while(1)
			{
				std::string result;
				std::string file = file_download_path + file_name;

				switch(statu)
				{
				case 0:
					result = send_request(s,"/version");
					new_version = result;
					logger().information("current version: " + current_version);
					logger().information("new version: "+ new_version);
					if(result.compare(current_version) == 0)
					{
						logger().information("version equal");
						return 0;
					}
					else
					{
						statu = 1;
					}
					break;
				case 1:
					result = send_request(s,"/upgrade");
					if(!result.empty())
					{
						logger().information("upload file");
						FILE* fp = fopen(file.c_str(),"w+");
						if(fp == NULL)
						{
							logger().error("open file failed");
							statu = -1;
							break;
						}
						int write_size = fwrite(result.c_str(),1,result.length(),fp);
						if(write_size < 0)
						{
							logger().error("write file failed");
							statu = -1;
						}
						fclose(fp);
						statu = 2;
					}
					break;
				case 2:
					result = send_request(s,"/md5");
					if(!result.empty())
					{
						logger().information("md5: " + result);
						std::string md5 = calculate_md5_value(file.c_str());
						if(!md5.empty() && md5.compare(result) == 0)
						{
							logger().information("md5 equal");
							statu = 3;
							break;
						}
					}
					statu = -1;
					break;
				case 3:
					logger().information("firmware upgrade sucess!");
					{
						std::string file_path = file_storage_path+file_name;
						std::string cmd = "cp " + file +" " + file_path;
						system(cmd.c_str());
						chmod(file_path.c_str(),S_IXUSR|S_IXGRP|S_IXOTH);
						//修改配置文件后，需通过save保存到文件中
						pConf->setString("Firmware.version[@value]",new_version);
						pConf->save("HTTPClient.xml");
					}
					statu = 0;
					break;
				case -1:
					logger().error("firmware upgrade failed!");
					statu = 0;
					break;
				default:
					break;
				}
				if(statu == 0)
				{
					break;
				}
			}
		}
		catch(Exception &e)
		{
			logger().error(e.message());
		}
		return Application::EXIT_OK;
	}
};

int main(int argc, char** argv)
{
	HttpAplication app;
	return app.run(argc,argv);
}

