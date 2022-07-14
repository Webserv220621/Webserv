#include <iostream>
#include "../Request.hpp"

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
			std::cout << "is_valid = " << std::boolalpha << rq.isValid() << std::endl;
	}

	if (!rq.isValid())
		return;

	if (rq.getState() > 0) {
		std::cout << "method: " << rq.getMethod() << std::endl;
		std::cout << "uri: " << rq.getUri().getRaw() << std::endl;
		std::cout << "version: " << rq.getVersion() << std::endl;
	}
	if (rq.getState() > 1) {
		std::cout << "----- headers -----" << std::endl;
		std::map<std::string, std::string> headers = rq.getAllHeaders();
		std::map<std::string,std::string>::iterator it;
		for (it = headers.begin(); it != headers.end(); ++it)
			std::cout << (*it).first << "===" << (*it).second << std::endl;
	}
	if (rq.getState() > 2) {
		std::cout << "----- body -----" << std::endl;
		std::cout << rq.getBody() << std::endl;
	}
}

int main() {
	std::string str;

	Request rq0;
	str = "GET / H\r\n";
	rq0.append_msg(str.c_str());
	std::cout << "state of rq0" << std::endl;
	prn_info(rq0);

	Request rq1;
	str = "GET     /    HTTP/1.";
	rq1.append_msg(str.c_str());
	std::cout << "\n\nstate of rq1" << std::endl;
	prn_info(rq1);

	str = "1\r\nhost:abc.com\r\nanother-header:something\r\n";
	rq1.append_msg(str.c_str());
	std::cout << "\n\nstate of rq1" << std::endl;
	prn_info(rq1);

	str = "\r\n";
	rq1.append_msg(str.c_str());
	std::cout << "\n\nstate of rq1" << std::endl;
	prn_info(rq1);


	Request rq2;
	str = "POST / HTTP/1.1\r\nhost:abc.com\r\ncontent-length:5\r\n\r\nabcd";
	rq2.append_msg(str.c_str());
	std::cout << "\n\nstate of rq2" << std::endl;
	prn_info(rq2);

	str = "e";
	rq2.append_msg(str.c_str());
	std::cout << "\n\nstate of rq2" << std::endl;
	prn_info(rq2);

std::cout << "\n\n\n";
	Request rq3;
	str = "POST / HTTP/1.1\r\nhost:abc.com\r\ntransfer-encoding:chunked\r\n\r\n5\r\nabcde\r\n3\r\nfgh\r\n0\r\n\r\n";
	rq3.append_msg(str.c_str());
	std::cout << "\n\nstate of rq3" << std::endl;
	prn_info(rq3);

	return 0;
}
