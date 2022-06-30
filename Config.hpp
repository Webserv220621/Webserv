#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

typedef struct location
{
    std::string _root;
    std::string _index;
    std::vector<std::string> _allowmethod;
    int _bodysize;
    int _autoindex; //off = 0, on = 1
};

class Config
{
private:
    std::string m_host;
    std::string m_port;
    std::string m_servername;
    std::map<std::string, location> m_location; //std::string에 location directory->다시
public:
    Config(/* args */);
    Config(const Config &other);
    ~Config();
    Config &operator=(const Config &other);

    std::string getHost();
    std::string getPort();
    std::string getServername();

    void setHost(std::string host);
    void setPort(std::string port);
    void setServername(std::string servername);
    
    void parsingConfig(std::string path);
};


#endif