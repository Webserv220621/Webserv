#include "Response.hpp"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctime>
#include <dirent.h>

//code -> m_code;

bool Response::isDirectory(std::string path)//https://www.it-note.kr/173, https://bubble-dev.tistory.com/entry/CC-stat2-lstat2-fstat2
{
    struct stat tmp;
    
    if (stat(path.c_str(), &tmp) != 0)
        return false;
    else if (tmp.st_mode & S_IFDIR)
        return true;
    else
        return false;
}

int Response::isExit(std::string path)
{
    struct  stat tmp;

    return (stat(path.c_str(), &tmp) == 0);
}

void Response::makeResponse()
{
    struct stat file;

    if (isDirectory(m_path))
    {
        if (_autoindex == 1)
            return (makeAutoIndex());
    }
    if (!isExist(m_path))
        return (makeErrorResponse(404));
    //에러 경우 추가
    
    //성공 & autoindex 없음
    addResponseLine(200);
    addDate();
    addContentLength((int)(file.st_size));
    addContenttype("txt/html");
}

void Response::addResponseLine(int status)
{
    m_responseMsg += "HTTP/1.1 " + std::to_string(status) + " ";
    m_responseMsg += m_statusMsg[status] + "\r\n"; //status message;
}

void Response::addDate()
{
    char    tmp[100];
    time_t timer = time(NULL);
    struct tm* t = localtime(&timer);

    m_responseMsg += "Date: ";
    strftime(tmp, 100, "%a, %d %b %z %H:%M:%S GMT", t);
    m_responseMsg += std::string(tmp) + "\r\n";

}

void Response::addEmptyLine()
{
    m_responseMsg += "\r\n";
}

void Response::addContenttype(std::string type)
{
    m_responseMsg += "Content-type: " + type + "\r\n";
}

void Response::addContentLength(int size)
{
    m_responseMsg += "Content-Length: " + std::to_string(size) + "\r\n";
}

void Response::addServer()
{
    m_responseMsg += "Server: " + severname + "\r\n";
}

void Response::addDirectory(std::string &body)//https://qdmana.com/2021/08/20210803055830874l.html
{
    std::string http_host_port = "http://" + m_host + m_port;
    if (http_host_port[http_host_port.length() - 1] != '/')
        http_host_port += "/";
    
    DIR *dir;
    struct dirent *diread = NULL;

    body += "<h1>Index of /autoindex/</h1><hr><pre><a href=../>../</a>";
    if ((dir = opendir(m_path)) == NULL) 
    {
        makeErrorResponse();
    }
    while (diread = readdir(dir))
    {
        std::string file_name(diread->d_name);
		if (file_name != "." && file_name != "..")
		{
			body += "<a href=" + http_host_port + file_name + " >" + file_name + "</a><br>";
		}
    }
    closedir(dir);
}

void Response::makeAutoIndex()//200 response.cpp 에 연결 137
{
    std::string html = "";
    html += "<!DOCTYPE html>\n";
    html += "<html>\n";
    html += "<head>\n";
    html += "</head>\n";
    html += "<body>\n";
    addDirectory(html); //directory를 html에 저장
    html += "</body>\n";
    html += "</html>\n";
    html += "\r\n";

    m_responseMsg += html;
}

void Response::makeErrorResponse(int error)//261
{
    std::string html = "";
	html += "<!DOCTYPE html>\n";
	html += "<html>\n";
	html += "<head>\n";
	html += "</head>\n";
	html += "<body>";
	html += std::to_string(error);
	html += "</body>\n";
	html += "</html>\n";

	m_responseMsg += html;
}