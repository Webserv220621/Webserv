#include "Config.hpp"

int main(int argc, char *argv[])//혹시 몰라 만든 메인문
{
    Webserv web;
    
    if (argc != 2)
    {
        std::cout << "Your input is wrong." << std::endl;
        std::cout << "it read <default.conf>" << std::endl;
		web.parsingWebserv(std::string("default.conf"));
    }
    else
    {
        web.parsingWebserv(std::string(argv[1]));
    }
    // for (int i = 0; i < web.getServerList().size(); i++)
    // {
    //     web.getServerList()[i].printServer();
    // }

    web.run();
}
