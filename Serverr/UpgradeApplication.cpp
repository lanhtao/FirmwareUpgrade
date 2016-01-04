#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <execinfo.h>
#include <sstream>
#include <iostream>
#include <string>
#include "UpgradeApplication.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/ThreadPool.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServer.h"
#include "UpgradeRequestHandlerFactory.h"

using Poco::ThreadPool;
using Poco::Net::HTTPServerParams;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Net::HTTPServer;
using namespace std;

// when program error send error info to this mail address
std::string g_administratorMail = "";
std::string g_host = "";
static void print_statck(int signum)
{
	void *array[10];
	int size;
	int i;
	char **strings;
	signal(signum, SIG_DFL);

	FILE *fp;
	fp = fopen("./content.txt", "w+");

	size = backtrace(array, 10);
	strings = (char **)backtrace_symbols(array,size);

	fputs("Received SIGEGV! stack trace:\n",fp);
	perror("Received SIGEGV! stack trace:\n");
	for(i = 0; i < size ;i++)
	{
		fputs(strings[i],fp);
		fputs("\n",fp);
	}
	fputs("FcgiServer Ver:", fp);
	fputs(VERSION, fp);
	fputs("\n",fp);

	fputs("host :", fp);
	fputs(g_host.c_str(), fp);
	fputs("\n",fp);

	free(strings);
	fclose(fp);

	if(g_administratorMail == "")
	{
		return;
	}
	char command[1024*10];
	sprintf(command,"mail -s \"Video Server error\" %s < content.txt", g_administratorMail.c_str());
	system(command);
}

UpgradeApplication::UpgradeApplication()
{
	port_ = (unsigned short)config().getInt("HTTPServer.port",8889);;
}

UpgradeApplication::~UpgradeApplication()
{

}

void UpgradeApplication::initialize(Application& self)
{
	ServerApplication::loadConfiguration();
	ServerApplication::initialize(self);

}

void UpgradeApplication::uninitialize()
{
	ServerApplication::uninitialize();
}

int UpgradeApplication::main(const std::vector<std::string>& args)
{
	//崩溃堆栈信息发送邮件
	signal(SIGSEGV, print_statck);
	signal(SIGABRT, print_statck);

	// get parameters from configuration file
	unsigned short port = (unsigned short) config().getInt("HTTPTimeServer.port", 8886);
	int maxQueued  = config().getInt("Thread.maxQueued", 100);
	int maxThreads = config().getInt("Thread.maxThreads", 16);
	ThreadPool::defaultPool().addCapacity(maxThreads);

	HTTPServerParams* pParams = new HTTPServerParams;
	pParams->setMaxQueued(maxQueued);
	pParams->setMaxThreads(maxThreads);

	 // set-up a server socket
	 ServerSocket svs(port);
	 // set-up a HTTPServer instance
	 HTTPServer srv(new UpgradeRequestHandlerFactory(), svs, pParams);
	 // start the HTTPServer
	 srv.start();
	 // wait for CTRL-C or kill
	 waitForTerminationRequest();
	 // Stop the HTTPServer
	 srv.stop();

     return Application::EXIT_OK;
}
