#include "Config.hpp"

Webserv::Webserv(){}

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
    Server      m_tmpserv;//변수명 변경 예정 

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
    {
        std::cout << "wrong config" << std::endl;
        return ;
    }
}

bool Webserv::checkWrongserv()//host와 port가 동일하면 return 0
{
    if (m_serv.size() < 2)
        return (1);
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