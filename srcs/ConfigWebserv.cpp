#include "Config.hpp"

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
        return ;
    }
    while (std::getline(fout, tmp)) // 한줄씩 읽어서 저장
    {
        buf += tmp;//엔터 없이 저장됨 (주석 포함)
    }
    ft_split(buf, info);// info에 띄어쓰기, 탭, 괄호 단위로 저장됨 (주석 포함)
    it = info.begin();
    while (it++ != info.end())
    {
        m_tmpserv.parsingServer(it, info.end());
        m_server_list.push_back(m_tmpserv);
        while (*it != "server" && it != info.end())
            it++;
    }
    if (checkWrongserv() == 0)//에러 처리 필요
    {
        std::cout << "wrong config" << std::endl;
        return ;
    }
    m_server_cnt = m_server_list.size();
}

bool Webserv::checkWrongserv()//host와 port가 동일하면 return 0
{
    if (m_server_list.size() < 2)
        return (1);
    for (size_t i = 0; i < m_server_list.size(); i++)
    {
        for (size_t j = i + 1; j < m_server_list.size(); j++)
        {
            if (m_server_list[i].getHost() == m_server_list[j].getHost())
            {
                if (m_server_list[i].getPort() == m_server_list[j].getPort())
                    return (0);
            }
        }
    }
    return (1);
}

std::vector<Server> Webserv::getServerList()
{
    return m_server_list;
}