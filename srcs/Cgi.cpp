#include "Cgi.hpp"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//PATH_INFO와 PATH_TRANSLATED, SCRIPT_NAME 주의 
Cgi::Cgi()
{
    // POST localhost:8080/html/abc.bla?v=1
    //http://lemp.test/test.php/foo/bar.php?v=1
    m_env["QUERY_STRING"] = "v=1";            // URL, 쿼리부분
    m_env["REQUEST_METHOD"] = "POST";      //임시
    m_env["CONTENT_TYPE"] = "txt/plain";  // 리퀘스트 헤더에서 가져와야함 
    m_env["CONTENT_LENGTH"] = "10";   //요청 바디 사이즈

    m_env["SCRIPT_FILENAME"] = "/Users/rhee/Documents/42seoul/web/abc.bla";  //    /var/www/test.php
    m_env["SCRIPT_NAME"] = "/html/abc.bla";      //    /test.php
    m_env["PATH_INFO"] = "/Users/rhee/Documents/42seoul/web/abc.bla";      //   /foo/bar.php
	m_env["PATH_TRANSLATED"] = "/Users/rhee/Documents/42seoul/web/abc.bla";  // 파일시스템 기반의 경로 
	m_env["REQUEST_URI"] = "/Users/rhee/Documents/42seoul/web/abc.bla"; ///test.php/foo/bar.php?v=1
   
    m_env["SERVER_PROTOCOL"] = "HTTP/1.1";
    m_env["GATEWAY_INTERFACE"] = "CGI/1.1";
    m_env["SERVER_SOFTWARE"] = "Webserv/1.0";

    m_env["REMOTEADRR"] = "8.0.0.1";
    m_env["SERVER_PORT"] = "8080";
    m_env["SERVER_NAME"] = "localhost";
    //https://www.php.net/manual/en/reserved.variables.server.php

	// m_env["AUTH_TYPE"]
	// fastcgi_param   DOCUMENT_URI            $document_uri;
    //$uri는 host부분과 arg부분을 제외한 영역으로 보시면 됩니다. 같은 것으로 document_uri가 있다고 하네요.
    // fastcgi_param   DOCUMENT_ROOT           $document_root; //웹 상에서 최상위 폴더가 되는 OS 상의 절대경로
    // fastcgi_param   REMOTE_ADDR             $remote_addr; //접속자의 ip
    // fastcgi_param   REMOTE_PORT             $remote_port; //접속자의 port
    // fastcgi_param   SERVER_ADDR             $server_addr; // 서버의 ip

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

std::string		Cgi::runCgi(std::string cgiPath) {
	pid_t		pid;
	char		**env;
	std::string	retCgi;
	int 		fd[2];
	
	env = envToChar();
	FILE *tmp = tmpfile();
	int cgiInput = fileno(tmp);
	write(cgiInput, "1234", 5); // requestMsg 넣어줄 예정
	if (pipe(fd) == -1)
	{
		deleteEnv(env);
		return ("Status: 500\r\n\r\n");
	}
	if ((pid= fork()) == -1)
	{
		deleteEnv(env);
		return ("Status: 500\r\n\r\n");
	}
	else if (pid == 0) // 자식 
	{
		close(fd[0]);
		dup2(cgiInput, STDIN_FILENO);
		dup2(fd[1], STDOUT_FILENO);
		if(execve(cgiPath.c_str(), NULL, env) == -1)
		{
			deleteEnv(env);
			return ("Status: 500\r\n\r\n");
		}
	}
	else // 부모
	{
		wait(NULL);
		char	buffer[32768] = {0}; // 32kb
		close(fd[1]);
		while (read(fd[0], buffer, 32768) > 0){
			read(fd[0], buffer, 32768);
			retCgi += buffer;
			memset(buffer, 0, 32768);
		}
	}
	close(fd[0]);
	deleteEnv(env);
	return (retCgi);
}

void Cgi::deleteEnv(char ** env){
	if (env != NULL)
	{
		for (int i = 0; env[i]; i++)
			delete[] env[i];
		delete[] env;
	}
}


// int main(){
//     Cgi c;

//     std::string buf;
//     buf = c.runCgi("../html/cgi/cgi_tester");
// 	std::cout << buf; 
//     return (0);
// }
// // ----
// // Status: 200 OK
// // Content-Type: text/html; charset=utf-8
// // 빈칸
// // ---