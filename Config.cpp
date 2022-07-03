#include "Config.hpp"

void ft_split(std::string buf, std::vector<std::string> &info)//다른 곳으로 이동 예정
{
    std::string exp = " \t{}";//예외
    std::string tmp; // vector에 넣어줄 값 

    for (std::string::iterator it = buf.begin(); it != buf.end(); it++)
    {
        if (exp.find(*it) == std::string::npos)
            tmp += *it;
        else
        {
            if (tmp != "")
            {
                info.push_back(tmp);
                tmp.clear();
            }
        }
    }
    if (tmp != "")
    {
        info.push_back(tmp);
        tmp.clear();
    }
}

std::string checkSemicolon(std::string input)
{
    std::string::iterator it;
    it = input.end();
    it--;
    if (*it == ';')
        input.erase(it);
    return (input);
}

/*
*/

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
                    if (*it == "off;")
                        t_location._autoindex = 0;
                    else
                        t_location._autoindex = 1;
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


/*
*/

Webserv::Webserv(){}

Webserv::Webserv(const Webserv &other)
{
    *this = other;
}

Webserv& Webserv::operator=(const Webserv &other)
{
    this->m_serv = other.m_serv;
    return (*this);
}

Webserv::~Webserv()
{
    m_serv.clear();
}

void Webserv::parsingWebserv(std::string path)
{
    std::ifstream   fout; // 파일 입력 객체
    std::string     buf; // 저장할 것
    std::string     tmp; // 임시
    std::vector<std::string>    info; // 구분용
    std::vector<std::string>::iterator it;

    fout.open(path, std::ofstream::in);
    if (!fout.is_open())
    {
        std::cout << "Can't open" << std::endl;
        return ; //에러 처리 필요
    }
    while (std::getline(fout, tmp)) // 한줄씩 읽어서 저장
    {
        buf += tmp;//엔터 없이 저장됨 (주석 포함)
    }
    //tab으로 구분 {,}도 저장하면 안댐
    ft_split(buf, info);// info에 띄어쓰기, 탭, 괄호 단위로 저장됨 (주석 포함)
    it = info.begin();
    while (it++ != info.end())
    {
        m_tmpserv.parsingServer(it, info.end());// 서버 블럭 처음과 끝 넣어주기->어케 나누지
        m_serv.push_back(m_tmpserv);
        while (*it != "server" && it != info.end())
            it++;
    }
    if (checkWrongserv() == 0)//에러 처리 필요
        std::cout << "wrong config" << std::endl;
}

bool Webserv::checkWrongserv()//host와 port가 동일하면 return 0
{
    for (size_t i = 0; i < m_serv.size(); i++)
    {
        for (size_t j = i; j < m_serv.size(); j++)
        {
            if (m_serv[i].getHost() == m_serv[j].getHost())
            {
                if (m_serv[i].getPort() == m_serv[j].getPort())
                    return (0);
            }
        }
    }
    return (1);
}

/*
*/

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