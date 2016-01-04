#include <iterator>
#include "UpgradeRequestHandler.h"
#include "Poco/Util/ServerApplication.h"
#include "Util.h"
using Poco::Util::ServerApplication;
using Poco::Util::Application;
UpgradeRequestHandler::UpgradeRequestHandler()
{

}

UpgradeRequestHandler::~UpgradeRequestHandler()
{
	// TODO Auto-generated destructor stub
}

int UpgradeRequestHandler::server_response(int type, HTTPServerResponse& response)
{
	Application& app = Application::instance();
	std::string version = app.config().getString("Firmware.version","1.0.0");
	std::string fileName = app.config().getString("Firmware.FileName","./newfirmware");
    response.setChunkedTransferEncoding(true);
    response.setContentType("text/html");
    //response 输入流
    std::ostream& ostr = response.send();
    switch(type)
    {
    case 0:
    	ostr<< version;
    	break;
    case 1:
    	ostr<< Util::calculate_md5_value(fileName.c_str());
    	break;
    case 2:
    	char read_buf[1024] = {0};
    	int read_size = 0;
	    int fd = open(fileName.c_str(),O_RDONLY);
	    if(fd < 0)
	    {
	    	app.logger().error("open file error");
	    }
	    do
	    {
	    	read_size = read(fd,read_buf,sizeof(read_buf));
	    	app.logger().information("read_size:%i?",read_size);
	    	if(read_size > 0)
	    		ostr << std::string(read_buf,read_size);
	    }while(read_size == 1024);
    	break;
    }
    return 0;
}

void UpgradeRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
    Application& app = Application::instance();
    std::string request_type[] ={ "version",
    							  "md5",
								  "upgrade"
    							};
    app.logger().information("	Request from " + request.clientAddress().toString());
    std::string uri = request.getURI();
    for(int i = 0; i<3; ++i)
    {
		if(uri.find(request_type[i]) != std::string::npos)
		{
			server_response(i,response);
			break ;
		}
    }
}
