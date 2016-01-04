#ifndef SRC_UPGRADEREQUESTHANDLER_H_
#define SRC_UPGRADEREQUESTHANDLER_H_
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPRequestHandler;

class UpgradeRequestHandler: public HTTPRequestHandler
{
public:
	UpgradeRequestHandler();

	virtual ~UpgradeRequestHandler();

	void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response);

private:
	int server_response(int type, HTTPServerResponse& response);
	std::string    calculate_md5_value(const char* fileName);
};

#endif /* SRC_UPGRADEREQUESTHANDLER_H_ */
