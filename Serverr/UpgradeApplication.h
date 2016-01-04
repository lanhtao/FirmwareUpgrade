#ifndef SRC_UPGRADEAPPLICATION_H_
#define SRC_UPGRADEAPPLICATION_H_

#include "Poco/Util/ServerApplication.h"

using Poco::Util::ServerApplication;


class HttpServer;
#define VERSION "1.0.0"

class UpgradeApplication:public ServerApplication
{
public:
	UpgradeApplication();
	virtual ~UpgradeApplication();
protected:
	void initialize(Application& self);

	void uninitialize();

	int main(const std::vector<std::string>& args);

private:
	unsigned short 	port_;
};

#endif /* SRC_UPGRADEAPPLICATION_H_ */
