#ifndef UTILL_HPP
#define UTILL_HPP

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <sys/time.h>

std::string checkSemicolon(std::string input);
void ft_split(std::string buf, std::vector<std::string> &info);
std::string& trimSpace(std::string& str);
void prn_prepend(const std::string& str, const std::string& ch);
unsigned int millisec(struct timeval& from, struct timeval& to);
template<typename T>
std::string ft_to_string(const T & value);
static int ft_stoi(const std::string & s);

#endif
