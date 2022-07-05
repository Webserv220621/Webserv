#include "Util.hpp"

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
