#ifndef SRC_UPGRADEREQUESTHANDLERFACTORY_H_
#define SRC_UPGRADEREQUESTHANDLERFACTORY_H_

#include "Poco/Net/HTTPRequestHandlerFactory.h"
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
class UpgradeRequestHandlerFactory:public HTTPRequestHandlerFactory
{
public:
	UpgradeRequestHandlerFactory();
	virtual ~UpgradeRequestHandlerFactory();
	HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request);
};

#endif /* SRC_UPGRADEREQUESTHANDLERFACTORY_H_ */
