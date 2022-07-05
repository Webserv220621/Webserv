#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <map>

#include "Util.hpp"

struct location
{
    std::string _root;
    std::string _index;
    std::string _cgipath;
    std::vector<std::string> _allowmethod;
    int _bodysize;
    int _autoindex; //off,init = 0, on = 1
};

class Server //ConfigSever.cpp
{
private:
    std::string m_host;
    int         m_port;
    std::string m_servername;
    std::map<std::string, location> m_location; //string에 location directory
    std::map<int, std::string> m_error;

public:
    Server();
    Server(const Server &other);
    ~Server();
    Server &operator=(const Server &other);
    
    std::string getHost()
    {
        return m_host;
    }
    int getPort()
    {
        return m_port;
    }
    std::string getServername()
    {
        return m_servername;
    }

    void initLocation(location *loc);
    void parsingServer(std::vector<std::string>::iterator it, std::vector<std::string>::iterator end);
    void printServer();// 임시 출력용
};

class Webserv // ConfigWebserv.cpp
{
    private:
        Webserv(const Webserv &other);
        Webserv &operator=(const Webserv &other);

        std::vector<Server> m_serv;
    public:
        Webserv();
        ~Webserv();

        void parsingWebserv(std::string path);
        bool checkWrongserv();
};

#endif