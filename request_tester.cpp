#include <iostream>
#include "Request.hpp"

void prn_info(Request& rq) {
	std::cout << "current state: ";
	switch(rq.getState()) {
		case 0:
			std::cout << "Reading Startline" << std::endl;
			break;
		case 1:
			std::cout << "Reading headers" << std::endl;
			break;
		case 2:
			std::cout << "Reading Body" << std::endl;
			break;
		case 3:
			std::cout << "receive end" << std::endl;
	}

	if (rq.getState() > 0) {
		std::cout << "method: " << rq.getMethod() << std::endl;
		std::cout << "uri: " << rq.getUri() << std::endl;
		std::cout << "version: " << rq.getVersion() << std::endl;
	}
	if (rq.getState() > 1) {
		std::cout << "----- headers -----" << std::endl;
		std::map<std::string, std::string> headers = rq.getAllHeaders();
		std::map<std::string,std::string>::iterator it;
		for (it = headers.begin(); it != headers.end(); ++it)
			std::cout << (*it).first << "===" << (*it).second << std::endl;
	}
}

int main() {
	Request rq1;



	std::string str = "GET     /    HTTP/1.";
	rq1.append(str);
	prn_info(rq1);

	str = "1\r\nhost:abc.com\r\ncontent-length:9\r\n";
	rq1.append(str);
	prn_info(rq1);

	str = "\r\n";
	rq1.append(str);
	prn_info(rq1);

	return 0;
}
