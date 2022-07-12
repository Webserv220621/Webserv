#include "Config.hpp"

int main(int argc, char *argv[])//혹시 몰라 만든 메인문
{
    Webserv web;
    
    if (argc != 2)
    {
		web.parsingWebserv(std::string("default.conf"));
    }
    else
    {
        web.parsingWebserv(std::string(argv[1]));
    }
    web.run();
    return 0;
}
