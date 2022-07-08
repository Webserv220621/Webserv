#include "Response.hpp"
#include <sys/stat.h> // 파일인지 디렉토리인지 확인
#include <unistd.h>
#include <fstream> // 파일 입출력
#include <sstream> // 파일 입출력
#include <ios> // file open시 옵션 설정 
#include <stdio.h> // remove 함수
#include <dirent.h> // direct 정리 관련
#include "common.hpp"

Response::Response () {
    //initResponse();
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

std::map<int, std::string> Response::m_errorMsg = {
    {100, "Continue"},
    {200, "OK"},
	{201, "Created"},
    {204, "No contetnt"},
    {301, "Moved Permanently"},
	{400, "Bad Request"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
    {411, "Length Required"},
	{413, "Payload Too Large"},
    {414, "URI Too Long"},
    {501, "Not Implemented"},
    {505, "HTTP Version Not Supported"}
};

//------------tmp--------------
// getter
int              Response::getCode(){ return m_code; }
std::string		Response::getResponseMsg(void) { return m_responseMsg; }
size_t          Response::getSentBytes() { return m_sent_bytes; }

void Response::initResponse(Server& server, Request& request) {
    Cgi cgiWeb;

    m_requestPath = "";
    m_bodySize = 0;
    m_contentType = "";
    m_contentLength = "";
    m_connection = ""; 
    if (request.isValid()) {
        m_code = 0;
        m_location = findMatchingLocation(server, request);
        m_requestPath = m_location._root + request.getUri().getPath();
        m_autoIndex = m_location._autoindex;
        m_method = request.getMethod();
    }
    else
        m_code = request.getState();
    m_responseMsg = "";
    m_cgiPath =  m_location._cgipath;
    m_requestBody = request.getBody();
    m_host =request.getUri().getHost();
    m_port = request.getUri().getPort();
    cgiWeb.init(m_location, request);
    m_cgi = cgiWeb;
    m_body = "";
    m_sent_bytes = 0;
}

size_t          Response::setSentBytes(size_t n) {
    m_sent_bytes = n;
}

std::string		Response::writeStartLine(void){
    std::string	startLine;

    startLine = "HTTP/1.1 " + std::to_string(m_code) + " " + m_errorMsg[m_code] + "\r\n";
    return (startLine);
}

std::string		Response::writeHeader(void)
{
	std::string	header = "";

	if (m_contentLength != "")
		header += "Content-Length: " + m_contentLength + "\r\n";
	// FIXME: m_contentType이 공백으로 시작하는 듯
    if (m_contentType != "")
		header += "Content-Type: " + m_contentType + "\r\n";
	if (m_connection != "")
		header += "Connection: " + m_connection + "\r\n";
	return (header);
}

int Response::validCheck(void) {
    // 각종 리퀘스트에러
    if (m_code > 299)
        return m_code;

    // m_method 가 allowed_method 안에 있는지 체크해서 method not allowed 전송
    bool allowed = false;
    std::vector<std::string>::iterator it;
    for (it = m_location._allowmethod.begin(); it != m_location._allowmethod.end(); ++it)
        if (*it == m_method) {
            allowed = true;
            break;
        }
    if (allowed == false) {
        m_code = METHOD_NOT_ALLOWED;
        return m_code;
    }

    return 0;
}

void Response::runResponse () {
    if (validCheck() != 0) {
        std::cout << "error code=" << m_code << std::endl;
        makeErrorResponse(m_code);
        //TODO: 헤더 채우기
    }
    else {
        std::cout << "request:\n";
        std::cout << m_method << "   " << m_requestPath << std::endl;
        m_responseMsg = "we will make response for you\r\n";

        if (m_method == "GET")
            getMethod();
        else if (m_method == "HEAD")
            headMethod();
        else if (m_method == "POST")
            postMethod();
        else if (m_method == "DELETE")
            deleteMethod();
        else
            putMethod();
    }
    writeResponseMsg();
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
    if (is_exist == -1)
        m_code = 404;
    else
    {
        if (is_dir) // case 1 : Url이 디렉토리일 경우
        {
            indexHtml = m_requestPath + "/index.html";
            if (access(indexHtml.c_str(), F_OK) == 0) // 그 디렉토리에index.html이 있다면 => index.html
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
                    makeAutoIndex();
                }    
            }
        }
        else // case 2 : Url이 파일인 경우 
        {
            readFile.open(path, std::ifstream::in);
            readBuf << readFile.rdbuf();
            m_body = readBuf.str();
            readFile.close();
            m_code = 200;
        }
    }
	
}

std::vector<std::string> split(std::string input, char delimiter) {
    std::vector<std::string> answer;
    std::stringstream ss(input);
    std::string temp;
 
    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    }
 
    return answer;
}

void			Response::getMethod(void) {
    if (m_cgiPath != "")
	{   
        std::string retCgi = m_cgi.runCgi(m_cgiPath);
        std::vector <std::string> result; 
	    result = split(retCgi, '\n');
	    for (int i = 0; i < result.size(); i++){
      
      
      
      
            if (result[i].find("Status") != std::string::npos)
            {
                int start = result[i].find(" ");
                m_code = stoi(result[i].substr(start, 4));
            }
            else if (result[i].find("Content-Type") != std::string::npos)
            {
                int start = result[i].find(" ");
                int end = result[i].find(" ", start);
                m_contentType = result[i].substr(start, end-3);
            }
            else if (result[i] != "\r")
            {
                m_body += result[i];
                if (i != result.size()-1)
                    m_body += "\n";
            }
        }   
	}
	else
		handleGet();
}

void			Response::headMethod(void) {
    getMethod();
    m_body = "";
}

void Response::handlePost() {
    const char  *path;
    std::ofstream writeFile; 
    
    //html/acb/test.txt -> 에러처리 

    path = m_requestPath.c_str();
    writeFile.open(path, std::ios_base::out | std::ios_base::trunc);
    writeFile << m_requestBody;
    writeFile.close();
    m_code = 201;
}

void Response::handlePut() {
    const char  *path;
    std::ofstream writeFile; 
    int         is_exist;
    
    path = m_requestPath.c_str();
    is_exist = access(path, F_OK); // F_OK 옵션은 파일존재여부만 확인
    if (is_exist == 0){
        writeFile.open(path, std::ios_base::app | std::ios_base::out);
        writeFile << m_requestBody;
        writeFile.close();
        m_code = 204; // 200 or 204
    }
    else{
        writeFile.open(path, std::ios_base::out);
        writeFile << m_requestBody;
        writeFile.close();
        m_code = 201;
    }
}
    
void			Response::postMethod(void) {
    if (m_cgiPath != "")
	{
        std::string retCgi = m_cgi.runCgi(m_cgiPath);
        std::vector <std::string> result; 
        result = split(retCgi, '\n');
        for (int i = 0; i < result.size(); i++){
            if (result[i].find("Status") != std::string::npos)
            {
                int start = result[i].find(" ");
                m_code = stoi(result[i].substr(start, 4));
            }
            else if (result[i].find("Content-Type") != std::string::npos)
            {
                int start = result[i].find(" ");
                int end = result[i].find(" ", start);
                m_contentType = result[i].substr(start, end-3);
            }
            else if (result[i] != "\r")
            {
                m_body += result[i];
                if (i != result.size()-1)
                    m_body += "\n";
            }
        }   
	}
	else
	{
        handlePost();
    }
}

void			Response::putMethod(void) {
    if (m_cgiPath != "")
	{
        std::string retCgi = m_cgi.runCgi(m_cgiPath);
        std::vector <std::string> result; 
	    result = split(retCgi, '\n');
	    for (int i = 0; i < result.size(); i++){
            if (result[i].find("Status") != std::string::npos)
            {
                int start = result[i].find(" ");
                m_code = stoi(result[i].substr(start, 4));
            }
            else if (result[i].find("Content-Type") != std::string::npos)
            {
                int start = result[i].find(" ");
                int end = result[i].find(" ", start);
                m_contentType = result[i].substr(start, end-3);
            }
            else if (result[i] != "\r")
            {
                m_body += result[i];
                if (i != result.size()-1)
                    m_body += "\n";
            }
        }   
	}
	else
	{
        handlePut();
    }
}
// 아마도 명령을 성공적으로 수행할 것 같으나 아직은 실행하지 않은 경우 202 (Accepted) 상태 코드.
// 명령을 수행했고 더 이상 제공할 정보가 없는 경우 204 (No Content) 상태 코드.
// 명령을 수행했고 응답 메시지가 이후의 상태를 설명하는 경우 200 (OK) 상태 코드.
//https://developer.mozilla.org/ko/docs/Web/HTTP/Methods/DELETE
void			Response::deleteMethod(void) {
    struct stat buf;
    int         is_dir;
    const char  *path;
    
    // 없는 경우 에러 처리, 디렉토리이 이거나 파일이 없는 경우 에러처리 해줘야함
    path = m_requestPath.c_str();
    stat(path,&buf);
    is_dir = buf.st_mode & S_IFDIR;
	if (is_dir) // case 1 : Url이 디렉토리일 경우 -> 일단 에러 처리 403에러 Forbidden
	{
        m_code = 403;
        makeErrorResponse(403);
	}
	else // case 2 : Url이 파일인 경우 
    {
       if (remove(path) == 0)
			m_code = 204;
		else // -1 리턴
        {
			m_code = 403;
            makeErrorResponse(403);
        }
	}
}

std::string Response::writeBody () {
    return m_body;
}

void Response::writeResponseMsg(void) {
    m_responseMsg += writeStartLine();
    m_responseMsg += writeHeader();
    if (m_body != "")
        m_responseMsg += "\r\n" + m_body;
}

void Response::addDirectory(std::string &body)
{
    std::string http_host_port = "http://" + m_host + ":" + m_port;
    if (http_host_port[http_host_port.length() - 1] != '/')
        http_host_port += "/";
    
    DIR *dir;
    struct dirent *diread = NULL;

    body += "<h1>Index of /autoindex/</h1><hr><br>";
    if ((dir = opendir(m_requestPath.c_str())) == NULL) 
    {
        return makeErrorResponse(500);//server에서 잘못
    }
    while ((diread = readdir(dir)))
    {
        std::string file_name(diread->d_name);
		if (file_name != "." && file_name != "..")
		{
			body += "<a href=" + http_host_port + file_name + " >" + file_name + "</a><br>";//file_name이 주소로 하이퍼링크 연결
		}
    }
    closedir(dir);
}

void Response::makeAutoIndex()//200 
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

    m_body += html;
}

void Response::makeErrorResponse(int error)
{
    if (error < 300)
        return ;

    std::string html = "";
	html += "<!DOCTYPE html>\n";
	html += "<html>\n";
	html += "<head>\n";
	html += "</head>\n";
	html += "<body>";
	html += "<h1>" + std::to_string(error) + " ERROR PAGE</h1>"; // error ERROR PAGE 출력
	html += "</body>\n";
	html += "</html>\n";

	m_body += html;
}

Location Response::findMatchingLocation(Server& s, Request& rq) {
	const Server::locations_map_type& locations = s.getLocations();
	Server::locations_map_type::const_reverse_iterator rit;
	Server::locations_map_type::const_reverse_iterator rit_best_match;

    const std::string& uri = rq.getUri().getPath();
	bool prefix_found = false;
	for (rit = locations.rbegin(); rit != locations.rend(); ++rit) {
		const std::string& prefix = rit->first;
		if (prefix[0] != '*') {
			if (prefix_found)
				continue;
			size_t pos = uri.find(prefix);
			if (pos != 0)
				continue;
			rit_best_match = rit;
			prefix_found = true;
		}
		else {
			size_t pos = uri.find(prefix.substr(1));
			if (pos == std::string::npos)
				continue;
			if (pos + prefix.length() - 1 == uri.length()) {
				rit_best_match = rit;
				break;
			}
		}
	}
    return rit_best_match->second;
}

// 헤더 setting 하는 부분도 필요
// cgi 처리 부분도 필요
// url 에러 처리도 필요
/*
int main() {
    Response rp;
    std::vector<std::string> methods = {"GET", "HEAD", "POST", "DELETE", "PUT"};
    rp.setBody("asd  asd asd테스트중 d asd\n asd asd asd aasd ");
    rp.setMethod(methods[4]);
    rp.setPath("./test.txt");
    rp.runResponse();
    rp.writeResponseMsg();
    std::cout << "---Msg---" << std::endl;
    std::cout << rp.getMsg() << std::endl;
    std::cout << "---code---" << std::endl;
    std::cout << rp.getCode() << std::endl;


    // struct stat buf;
    // stat("testd", &buf); // 없으면 0, 있으면 파일 정보 
    // std::cout << (buf.st_mode & S_IFDIR) << std::endl;
    // std::cout << (buf.st_mode & S_IFREG) << std::endl;
    return 0;
}
*/
