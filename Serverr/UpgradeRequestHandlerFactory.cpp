#include "UpgradeRequestHandlerFactory.h"
#include "UpgradeRequestHandler.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"



UpgradeRequestHandlerFactory::UpgradeRequestHandlerFactory()
{

}

UpgradeRequestHandlerFactory::~UpgradeRequestHandlerFactory()
{

}

HTTPRequestHandler* UpgradeRequestHandlerFactory::createRequestHandler(const HTTPServerRequest& request)
{
        if (request.getURI().find("/") != std::string::npos)
                return new UpgradeRequestHandler();
        else
                return 0;
}
