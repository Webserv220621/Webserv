#include <iostream>
#include <assert.h>
#include "../utils/Uri.hpp"

void prn_uri(Uri& target) {
	std::cout << "scheme: " << target.getScheme() << std::endl;
	std::cout << "host: " << target.getHost() << std::endl;
	std::cout << "port: " << target.getPort() << std::endl;
	std::cout << "path: " << target.getPath() << std::endl;
	std::cout << "query: " << target.getQuery() << std::endl;
	std::cout << "-----" << std::endl;
}

int main() {
	Uri target("http://www.abc.com:80/path/to/file?key=value");
	prn_uri(target);
	assert(target.getScheme() == "http");
	assert(target.getHost() == "www.abc.com");
	assert(target.getPort() == "80");
	assert(target.getPath() == "/path/to/file");
	assert(target.getQuery() == "key=value");


	Uri t2("http://www.abc.com/path/to/dir/");
	prn_uri(t2);
	assert(t2.getScheme() == "http");
	assert(t2.getHost() == "www.abc.com");
	assert(t2.getPort() == "");
	assert(t2.getPath() == "/path/to/dir/");
	assert(t2.getQuery() == "");

	Uri t3("www.abc.com");
	prn_uri(t3);
	assert(t3.getScheme() == "");
	assert(t3.getHost() == "www.abc.com");
	assert(t3.getPort() == "");
	assert(t3.getPath() == "");
	assert(t3.getQuery() == "");

	Uri t4("/path_to_file");
	prn_uri(t4);
	assert(t4.getScheme() == "");
	assert(t4.getHost() == "");
	assert(t4.getPort() == "");
	assert(t4.getPath() == "/path_to_file");
	assert(t4.getQuery() == "");

	Uri t5("/path_to_dir/");
	prn_uri(t5);
	assert(t5.getScheme() == "");
	assert(t5.getHost() == "");
	assert(t5.getPort() == "");
	assert(t5.getPath() == "/path_to_dir/");
	assert(t5.getQuery() == "");


	return 0;
}
