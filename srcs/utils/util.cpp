#include "util.hpp"

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

std::string& trimSpace(std::string& str) {
    str.erase(0, str.find_first_not_of(' '));
    str.erase(str.find_last_not_of(' ') + 1);
    return str;

}

void prn_prepend(const std::string& str, const std::string& ch) {
    std::istringstream ss(str);
    std::string line;
    while (std::getline(ss, line, '\n'))
        std::cout << ch << line << std::endl;
}

unsigned int millisec(struct timeval& from, struct timeval& to) {
    return ((to.tv_sec - from.tv_sec) * 1000 + (to.tv_usec - from.tv_usec) / 1000);
}