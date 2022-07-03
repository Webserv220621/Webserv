#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <map>

struct location
{
    std::string _root;
    std::string _index;
    std::vector<std::string> _allowmethod;
    int _bodysize;
    int _autoindex; //off = 0, on = 1, init = -1
};

class Server
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

class Webserv
{
    private:
        Server              m_tmpserv;//m_serv에 넣어주려고 만든 임시 친구
        std::vector<Server> m_serv;

    public:
        Webserv();
        Webserv(const Webserv &other);
        ~Webserv();
        Webserv &operator=(const Webserv &other);

        void parsingWebserv(std::string path);
        bool checkWrongserv();
};

#endif