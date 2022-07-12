#ifndef CGI_HPP
# define CGI_HPP

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <string>
#include "Config.hpp"
#include "Request.hpp"


class Cgi {
    private:
		char								**envToChar();
		std::map<std::string, std::string>	m_env;
		std::string							m_requestMsg;

	public:
		Cgi()  {};
		~Cgi() {};
		std::string		                    runCgi(std::string cgiPath);
		void								deleteEnv(char **env);
		void								init(Location location, Request& request);

};
#endif