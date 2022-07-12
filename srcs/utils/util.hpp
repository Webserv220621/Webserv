#ifndef UTILL_HPP
#define UTILL_HPP

#include <string>
#include <vector>
#include <sstream>

std::string checkSemicolon(std::string input);
void ft_split(std::string buf, std::vector<std::string> &info);
std::string& trimSpace(std::string& str);
void prn_prepend(const std::string& str, const std::string& ch);
unsigned int millisec(struct timeval& from, struct timeval& to);
template<typename T>
std::string to_string(const T & value);
int stoi(std::string & s);

#endif
