#include "Response.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <ios>

Response::Response () {
    initResponse();
}
//------------tmp--------------
void Response::setPath(std::string path) {
    m_requestPath = path;
}

void Response::setMethod(std::string method) {
    m_method = method;
}

void			Response::setBody(std::string body){
    m_requestBody = body;
}


//------------tmp--------------
// getter
int              Response::getCode(){ return m_code; }
std::string		Response::getBody(void) { return m_body; }

void Response::initResponse() {
    m_requestPath = "";
    m_bodySize = 0;
    m_contentType = "";
    m_contentLength = "";
    m_connection = ""; 
    m_code = 0;
    m_autoIndex = 0;
    m_responseMsg = "";
    m_cgiPath =  "";
    m_method = "";
    m_requestBody = "";
    m_errorMsg[100] = "Continue";
	m_errorMsg[200] = "OK";
	m_errorMsg[201] = "Created";
	m_errorMsg[400] = "Bad Request";
	m_errorMsg[403] = "Forbidden";
	m_errorMsg[404] = "Not Found";
	m_errorMsg[405] = "Method Not Allowed";
	m_errorMsg[413] = "Payload Too Large";
}

std::string		Response::getStartLine(void){
    std::string	startLine;

    startLine = "HTTP/1.1 " + std::to_string(m_code) + " " + m_errorMsg[m_code] + "\r\n";
    return (startLine);
}

std::string		Response::getHeader(void)
{
	std::string	header = "";

	if (m_contentLength != "")
		header += "Content-Length: " + m_contentLength + "\r\n";
	if (m_contentType != "")
		header += "Content-Type: " + m_contentType + "\r\n";
	if (m_connection != "")
		header += "Connection: " + m_connection + "\r\n";
	return (header);
}

int Response::validCheck(void) {
    if (m_requestPath == "404")
        return 404;
    else if (0) // m_method 가 allowed_method 안에 있는지 체크해서 method not allowed 에러 출력
        return 405;
    return 0;
}

void Response::runResponse () {

    if (validCheck() != 0)
    {
        m_code = validCheck();
        writeBody();
    }
    else
    {
        if (m_method == "GET")
            getMethod();
        else if (m_method == "HEAD")
            headMethod();
        else if (m_method == "POST")
            postMethod();
        else if (m_method == "DELETE")
            deleteMethod();
    }
}

void             Response::handleGet(void) {
    struct stat buf;
    int         is_dir;
    int         is_exist;
    const char  *path;
    std::string indexHtml;
    std::ifstream readFile; 
    std::stringstream readBuf;
    
    path = m_requestPath.c_str();
    stat(path,&buf);
    is_dir = buf.st_mode & S_IFDIR;
    is_exist = access(path, F_OK); // F_OK 옵션은 파일존재여부만 확인
	if (is_dir) // case 1 : Url이 디렉토리일 경우
	{
        indexHtml = m_requestPath + "index,html";
        if (is_exist == -1)
            m_code = 404;
        else if (access(indexHtml.c_str(), F_OK) == 0) // 그 디렉토리에index.html이 있다면 => index.html
        {
            // file.open("index.html", );
            m_contentType = "txt/html";
        }
        else
        {
            if (m_autoIndex == 0) //index.html이 없는데 autoindex가 off다 => 403 Forbidden
            {
                m_code = 403;
            }
            else // index.html이 없는데 autoindex가 on이다 => 디렉토리 리스팅
            {  
                m_body = "";
                // m_body = makeIndexhtml();
            }    
        }
	}
	else // case 2 : Url이 파일인 경우 
    {
        if (is_exist == -1)
            m_code = 404;
        else
        {
            readFile.open(path, std::ifstream::in);
            readBuf << readFile.rdbuf();
            m_body = readBuf.str();
            readFile.close();
            m_code = 200;
        }
	}
}

void			Response::getMethod(void) {
    if (m_cgiPath != "")
	{
        // m_code = "Status : " 파싱 
        // m_contentType = "Content-type: " 파싱
		// m_body = processCgi();
	}
	else
		handleGet();
}

void			Response::headMethod(void) {
    getMethod();
    m_body = "";
}

void Response::handlePost() {
    struct stat buf;
    int         is_dir;
    const char  *path;
    std::ofstream writeFile; 
    
    path = m_requestPath.c_str();
    stat(path,&buf);
    is_dir = buf.st_mode & S_IFDIR;
	if (is_dir) // case 1 : Url이 디렉토리일 경우 -> 일단 에러 처리
	{
        m_code = 400;
	}
	else // case 2 : Url이 파일인 경우 
    {
        writeFile.open(path, std::ios_base::out | std::ios_base::trunc);
		writeFile << m_requestBody;
		writeFile.close();
		m_code = 201;
	}
}
void			Response::postMethod(void) {
    if (m_cgiPath != "")
	{
        // m_code = "Status : " 파싱 
        // m_contentType = "Content-type: " 파싱
		// m_body = processCgi();
	}
	else
	{
        handlePost();
    }
}

void			Response::deleteMethod(void) {

}

std::string Response::writeBody () {
    return "";
}

void Response::writeResponseMsg(void) {
    m_responseMsg += getStartLine();
    m_responseMsg += getHeader();
    m_responseMsg += writeBody();
}

int main() {
    Response rp;
    std::vector<std::string> methods = {"GET", "HEAD", "POST", "DELETE"};
    rp.setBody("asd  asd asd as dasd asd\n asd asd asd aasd ");
    rp.setMethod(methods[2]);
    rp.setPath("./test.txt");
    rp.runResponse();
    std::cout << "---body---" << std::endl;
    std::cout << rp.getBody() << std::endl;
    std::cout << "---code---" << std::endl;
    std::cout << rp.getCode() << std::endl;

    return 0;
}