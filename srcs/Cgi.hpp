#ifndef CGI_HPP
# define CGI_HPP

#include "Config.hpp"
#include "Request.hpp"
# include <string>
#include <unistd.h>

class Cgi {
    private:
		char								**envToChar();
		std::map<std::string, std::string>	m_env;
		std::string							m_requestMsg;

	public:
		Cgi();
		~Cgi() {};
		std::string		                    runCgi(std::string cgiPath);
		void								deleteEnv(char **env);
		void								init(Location location, Request& request);

};
#endif