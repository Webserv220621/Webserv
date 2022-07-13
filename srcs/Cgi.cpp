
#include "Cgi.hpp"

void					Cgi::init(Location location, Request& request){
	m_requestMsg = request.getBody();
    m_env["QUERY_STRING"] = request.getUri().getQuery();
    m_env["REQUEST_METHOD"] = request.getMethod();
    m_env["CONTENT_TYPE"] = request.getHeaderValue("Content-Type");
    m_env["CONTENT_LENGTH"] = ft_to_string(request.getBody().length());

    m_env["SCRIPT_FILENAME"] = location._root + request.getUri().getPath();
    m_env["SCRIPT_NAME"] = request.getUri().getPath();
    m_env["PATH_INFO"] =  request.getUri().getPath();
	m_env["PATH_TRANSLATED"] = location._root + request.getUri().getPath();
	m_env["REQUEST_URI"] = request.getUri().getPath() + request.getUri().getQuery();
    m_env["SERVER_PROTOCOL"] = "HTTP/1.1";
    m_env["GATEWAY_INTERFACE"] = "CGI/1.1";
    m_env["SERVER_SOFTWARE"] = "Webserv/1.0";
    m_env["SERVER_PORT"] = request.getUri().getPort();
    m_env["SERVER_NAME"] = request.getUri().getHost();

	/* request에서 앞에 x- 가 붙은 비표준 헤더들을 CGI ENV가 요구하는 형식으로 변환
	 * X-Secret-Header ---> HTTP_X_SECRET_HEADER
	*/
	std::map<std::string, std::string> all_headers_from_request = request.getAllHeaders();
	std::map<std::string, std::string>::iterator it;
	for (it = all_headers_from_request.begin(); it != all_headers_from_request.end(); ++it) {
		const std::string& old_key = it->first;
		if ( old_key.find("x-") == 0 ) {
			std::string new_key = "HTTP_";
			for (size_t i = 0; i < old_key.length(); ++i) {
				if (old_key[i] == '-')
					new_key.push_back('_');
				else
					new_key.push_back(std::toupper(old_key[i]));
			}
			m_env[new_key] = it->second;
		}
	}
}

char					**Cgi::envToChar() {
	int i = 0;
    std::map<std::string, std::string>::iterator it;
	char	**newEnv = new char*[m_env.size() + 1];
    
	for (it = m_env.begin(); it != m_env.end(); it++) {
		std::string	envContent = it->first + "=" + it->second;
		newEnv[i] = new char[envContent.size() + 1];
		strcpy(newEnv[i], (const char*)envContent.c_str());
		i++;
	}
	newEnv[i] = NULL;
	return newEnv;
}

std::string				Cgi::runCgi(std::string cgiPath) {
	pid_t		pid;
	char		**env;
	std::string	retCgi;
	int			stat;
	
	env = envToChar();
	FILE *tmp = tmpfile();
	FILE *msg = tmpfile();
	int storeMsg = fileno(msg);
	int cgiInput = fileno(tmp);
	if (write(cgiInput, m_requestMsg.c_str(), m_requestMsg.size()) == -1)
		return ("Status: 500\r\n\r\n");
	lseek(cgiInput, 0, 0);
	if ((pid= fork()) == -1)
	{
		deleteEnv(env);
		return ("Status: 500\r\n\r\n");
	}
	else if (pid == 0)
	{
		dup2(cgiInput, STDIN_FILENO);
		dup2(storeMsg, STDOUT_FILENO);
		if(execve(cgiPath.c_str(), (char**){NULL}, env) == -1)
		{
			deleteEnv(env);
			exit(1);	
		}
	}
	else
	{
		wait(&stat);
		if (WIFSIGNALED(stat))
			return ("Status: 500\r\n\r\n");
		char	buffer[32768] = {0};
		lseek(storeMsg, 0, 0);
		while (read(storeMsg, buffer, 32767) > 0){
			retCgi += buffer;
			memset(buffer, 0, 32768);
		}
	}
	fclose(tmp);
	fclose(msg);
	deleteEnv(env);
	return (retCgi);
}

void 					Cgi::deleteEnv(char ** env){
	if (env != NULL)
	{
		for (int i = 0; env[i]; i++)
			delete[] env[i];
		delete[] env;
	}
}
