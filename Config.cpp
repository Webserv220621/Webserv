#include "Config.hpp"

void ft_split(std::string buf, std::vector<std::string> &info)//다른 곳으로 이동 예정
{
    std::string exp = " \t{}";//예외
    std::string tmp; // vector에 넣어줄 값 

    for (std::string::iterator it = buf.begin(); it != buf.end(); it++)
    {
        if (exp.find(*it) == std::string::npos)
        {
            tmp += *it;
        }
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

std::string checkSemicolon(std::string input) // pop_back() -> c+11
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

Config::Config()
{
}

Config::Config(const Config &other)
{
    *this = other;
}

Config::~Config()
{
}

Config& Config::operator=(const Config &other)
{
    this->m_host = other.m_host;
    this->m_port = other.m_port;
    this->m_servername = other.m_servername;
    this->m_location = other.m_location;
    return (*this);
}

std::string Config::getHost()
{
    return (this->m_host);
};
std::string Config::getPort()
{
    return (this->m_port);
};
std::string Config::getServername()
{
    return (this->m_servername);
};

void Config::setHost(std::string host)
{
    this->m_host = host;
};
void Config::setPort(std::string port)
{
    this->m_port = port;
};
void Config::setServername(std::string servername)
{
    this->m_servername = servername;
};

void Config::parsingConfig(std::string path)
{
    std::ifstream fout; // 파일 입력 객체
    std::string   buf; // 저장할 것
    std::string   tmp; // 임시
    std::vector<std::string> info; // 구분용
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
    while (it != info.end())
    {
        if (*it == "host")
        {
            it++;
            tmp = (*it);
            tmp = checkSemicolon(tmp); //세미콜론 있으면 마지막 떼어주기
            setHost(tmp);
        }
        if (*it == "port")
        {
            it++;
            tmp = *it;
            tmp = checkSemicolon(tmp);
            setPort(tmp);
        }
        if (*it == "server_name")
        {
            it++;
            tmp = *it;
            tmp = checkSemicolon(tmp);
            setServername(tmp);
        }
        if (*it == "location")
        {
            it++;
            // 어떻게 처리 할 것인지 (location 여러개인 경우)
        }
        it++;
    }
    
}


/*
*/

int main(int argc, char *argv[])//혹시 몰라 만든 메인문
{
    Config config;
    
    if (argc != 2)
    {
        std::cout << "Invalid arg." << std::endl;
		return (1);
    }
    else
    {
        config.parsingConfig(std::string(argv[1]));
    }
}