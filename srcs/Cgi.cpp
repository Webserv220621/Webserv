
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

void					Cgi::runCgi(std::string cgiPath, std::string& retCgi) {
	pid_t		pid;
	char		**env;
	int 		pipe_to_cgi[2];
	int 		pipe_from_cgi[2];
	
#if DEBUG
    gettimeofday(&_start_time, NULL);
#endif
	env = envToChar();
	if (pipe(pipe_to_cgi) == -1)
	{
		deleteEnv(env);
		retCgi = "Status: 500\r\n\r\n";
		return;
	}
	if (pipe(pipe_from_cgi) == -1)
	{
		deleteEnv(env);
		retCgi = "Status: 500\r\n\r\n";
		return;
	}
	if ((pid= fork()) == -1)
	{
		deleteEnv(env);
		retCgi = "Status: 500\r\n\r\n";
		return;
	}
	else if (pid == 0) // 자식 
	{
		close(pipe_from_cgi[0]);
		close(pipe_to_cgi[1]);
		dup2(pipe_to_cgi[0], STDIN_FILENO);
		dup2(pipe_from_cgi[1], STDOUT_FILENO);
		if(execve(cgiPath.c_str(), (char**){NULL}, env) == -1)
		{
			deleteEnv(env);
			exit(1);
		}
	}
	else // 부모
	{
		close(pipe_from_cgi[1]);
		close(pipe_to_cgi[0]);
		size_t msglen = m_requestMsg.length();
		char buffer[32768];
		size_t sent_idx = 0;
		while (1) {
			size_t part_len = 32767;
			if (sent_idx + part_len > msglen)
				part_len = msglen - sent_idx;
			// 전송용 파이프의 write end에 쓰기
			write(pipe_to_cgi[1], m_requestMsg.substr(sent_idx, part_len).c_str(), part_len);
			sent_idx += part_len;
			// 전부 보냈으면 전송용 파이프의 write end를 닫는다
			if (sent_idx == msglen)
				close(pipe_to_cgi[1]);
			// 수신용 파이프의 read end로부터 읽기
			int ret = read(pipe_from_cgi[0], buffer, 32767);
			if (ret == 0)
				break;
			buffer[ret] = '\0';
			retCgi += buffer;
		}
	}
	close(pipe_from_cgi[0]);
	deleteEnv(env);
#if DEBUG
	struct timeval current;
	gettimeofday(&current, NULL);
	unsigned int elapsed = millisec(_start_time, current);
	std::cout << "   [ cgi process elapsed: " << elapsed << "ms ]" << std::endl;
#endif
	return;
}

void 					Cgi::deleteEnv(char ** env){
	if (env != NULL)
	{
		for (int i = 0; env[i]; i++)
			delete[] env[i];
		delete[] env;
	}
}
