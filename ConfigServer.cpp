#include "Config.hpp"

Server::Server(){}

Server::Server(const Server &other)
{
    *this = other;
}

Server::~Server()
{
    m_error.clear();
    m_location.clear();
}

Server& Server::operator=(const Server &other)
{
    this->m_host = other.m_host;
    this->m_port = other.m_port;
    this->m_servername = other.m_servername;
    this->m_location = other.m_location;
    this->m_error = other.m_error;
    return (*this);
}

void Server::initLocation(location *loc)
{
    loc->_allowmethod.clear();
    loc->_autoindex = -1;
    loc->_bodysize = 0;
    loc->_index = "";
    loc->_root = "";
    loc->_cgipath = "";
}

void Server::parsingServer(std::vector<std::string>::iterator it, std::vector<std::string>::iterator end)//begin(), end()
{
    std::string         tmp;
    location            t_location;
    std::vector<int>    errorNum;
    
    if (*it == "server")
        it++;
    if (m_location.size() > 0)
        m_location.clear();
    while (it != end && *it != "server")
    {
        if (*it == "host")
        {
            it++;
            m_host = checkSemicolon(*it); //세미콜론 있으면 마지막 떼어주기
        }
        if (*it == "port")
        {
            it++;
            m_port = atoi((*it).c_str());
        }
        if (*it == "server_name")
        {
            it++;
            m_servername = checkSemicolon(*it);
        }
        if (*it == "location")
        {
            it++;
            tmp = *it; // direct 저장
            initLocation(&t_location);
            it++;
            for (; *it != "location" && *it != "server" && *it != "error_page" && it != end ; it++) //location 개수만큼
            {
                if (*it == "root")
                {
                    it++;
                    t_location._root = checkSemicolon(*it);
                }
                else if (*it == "index")
                {
                    it++;
                    t_location._index = checkSemicolon(*it);                
                }
                else if (*it == "allowed_method")
                {
                    it++;
                    if (t_location._allowmethod.size() != 0)
                        t_location._allowmethod.clear();
                    while (*it == checkSemicolon(*it))
                    {
                        t_location._allowmethod.push_back(*it);
                        it++;
                    }
                    t_location._allowmethod.push_back(checkSemicolon(*it));
                }
                else if (*it == "limit_body_size")
                {
                    it++;
                    std::string tmp2 = *it;
                    int size = atoi(tmp2.c_str());
                    t_location._bodysize = size;
                }
                else if (*it == "autoindex")
                {
                    it++;
                    if (*it == "on;")
                        t_location._autoindex = 1;
                    else
                        t_location._autoindex = 0;
                }
                else if (*it == "cgi_path")
                {
                    it++;
                    t_location._cgipath = checkSemicolon(*it);
                }
                else
                {
                    if (*it != "")
                    {
                        tmp += " ";
                        tmp += *it;
                    }
                }
            }
            m_location.insert(make_pair(tmp, t_location));
            it--;
        }
        if (*it == "error_page")
        {
            it++;
            if (errorNum.size() > 0)
                errorNum.clear();
            while (*it == checkSemicolon(*it))
            {
                errorNum.push_back(atoi((*it).c_str()));
                it++;
            }
            std::string error_page = checkSemicolon(*it);
            for (int i = 0; i < errorNum.size(); i++)
            {
                m_error.insert(make_pair(errorNum[i], error_page));
            }
        }
        it++;
    }
}

void Server::printServer()//출력용
{
    std::map<std::string, location>::iterator it = m_location.begin();

    std::cout << "host : " << m_host << std::endl;
    std::cout << "port : " << m_port << std::endl;
    std::cout << "servername : " << m_servername << std::endl;
    std::cout << std::endl;
    std::cout <<  "location size : " << m_location.size() << std::endl;
    std::cout << std::endl;
    for (; it != m_location.end(); it++)
    {
        std::cout << "--------------------------" << std::endl;
        std::cout << "direct name : " << it->first << std::endl;
        std::cout << "root : " << it->second._root << std::endl;
        std::cout << "index : " << it->second._index << std::endl;
        std::cout << "cgi_path : " << it->second._cgipath << std::endl;
        std::cout << "bodysize : " << it->second._bodysize << std::endl;
        std::cout << "autoindex : " << it->second._autoindex << std::endl;
        std::cout << "allowmethod : ";
        for (int j = 0; j < it->second._allowmethod.size(); j++)
        {
            std::cout << it->second._allowmethod[j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::map<int, std::string>::iterator it2 = m_error.begin();
    std::cout << "error_page size : " << m_error.size() << std::endl; 
    for (int j = 0; j < m_error.size(); j++)
    {
        std::cout << "--------------------------" << std::endl;
        std::cout << "error_num : " << it2->first << std::endl;
        std::cout << "error_page : " << it2->second << std::endl;
        it2++;
    }
}