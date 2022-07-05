#include "Config.hpp"

int main(int argc, char *argv[])//혹시 몰라 만든 메인문
{
    Webserv web;
    
    if (argc != 2)
    {
        std::cout << "Invalid arg." << std::endl;
		return (1);
    }
    else
    {
        web.parsingWebserv(std::string(argv[1]));
    }
}