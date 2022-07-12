#include "Response.hpp"
#include "common.hpp"
#include "utils/util.hpp"

Response::Response () {
}

void            Response::setPath(std::string path) {
    m_requestPath = path;
}

void            Response::setMethod(std::string method) {
    m_method = method;
}

void			Response::setBody(std::string body){
    m_requestBody = body;
}

std::map<int, std::string>& initErrorMap() {
static std::map<int, std::string> tmp;
	tmp[100] = "Continue";
	tmp[200] = "OK";
	tmp[201] = "Created";
	tmp[204] = "No contetnt";
	tmp[301] = "Moved Permanently";
	tmp[400] = "Bad Request";
	tmp[403] = "Forbidden";
	tmp[404] = "Not Found";
	tmp[405] = "Method Not Allowed";
	tmp[411] = "Length Required";
	tmp[413] = "Payload Too Large";
	tmp[414] = "URI Too Long";
	tmp[500] = "Internal Server Error";
	tmp[501] = "Not Implemented";
	tmp[503] = "Service Unavailable";
	tmp[505] = "HTTP Version Not Supported";
	return tmp;
}

std::map<int, std::string> Response::m_errorMsg = initErrorMap();

// getter
int              Response::getCode(){ return m_code; }
std::string		Response::getResponseMsg(void) { return m_responseMsg; }
size_t          Response::getSentBytes() { return m_sent_bytes; }
bool            Response::isKeepAlive() {
    // 응답코드가 400, 50x면 연결종료
    if (m_code == BAD_REQUEST || m_code >= INTERNAL_SERVER_ERROR)
        return false;
    // TODO: 요청헤더의 connection이 명시적으로 close면 연결종료
    // TODO: 요청version이 1.0이고 요청헤더의 connection이 Keep-Alive가 아니면 연결종료
    return true;
}

void Response::initResponse(Server& server, Request& request) {
    Cgi cgiWeb;

    m_requestPath = "";
    m_bodySize = 0;
    m_contentType = "text/plain";
    m_contentLength = "";
    m_connection = "Keep-Alive";
    // m_connection = "Close";
    if (request.isValid()) {
        m_code = 0;
        m_location = findMatchingLocation(server, request);
        if (m_location._bodysize && request.getBody().size() > static_cast<size_t>(m_location._bodysize)){
            m_code = 413;
        }
        m_uripath = request.getUri().getPath();
        if (m_location._prefix[0] == '*')
            m_requestPath = m_uripath;
        else if (m_location._prefix.length() > 1)
            m_requestPath = m_location._root + m_uripath.substr(m_location._prefix.length());
        else
            m_requestPath = m_location._root + request.getUri().getPath();
        m_query = request.getUri().getQuery();
        m_autoIndex = m_location._autoindex;
        m_method = request.getMethod();
    }
    else
        m_code = request.getState();
    m_responseMsg = "";
    m_cgiPath =  m_location._cgipath;
    // FIXME: cgi_ext 설정 추가
    if (m_requestPath.substr(m_requestPath.find_last_of(".")+1) != "bla" || m_method != "POST")
        m_cgiPath = "";
    m_requestBody = request.getBody();
    m_host =request.getUri().getHost();
    m_port = request.getUri().getPort();
    // uri에 host, port 정보가 없으면 server 정보를 적용
    // TODO: servername이 있으면 servername 우선
    if (m_host == "")
        m_host = server.getHost();
    if (m_port == "")
        m_port = ft_to_string(server.getPort());
    cgiWeb.init(m_location, request);
    m_cgi = cgiWeb;
    m_body = "";
    m_sent_bytes = 0;
    m_indexFile = m_location._index;
    // std::cout << "     [ param ]" << std::endl;
    // std::cout << " method: " << m_method << std::endl;
    // std::cout << " targetPath: " << m_requestPath << std::endl;
}

void          Response::setSentBytes(size_t n) {
    m_sent_bytes = n;
}

std::string		Response::writeStartLine(void){
    std::string	startLine;

    startLine = "HTTP/1.1 " + ft_to_string(m_code) + " " + m_errorMsg[m_code] + "\r\n";
    return (startLine);
}

std::string		Response::writeHeader(void)
{
	std::string	header = "";

    if (m_contentLength == "")
	    header += "Content-Length: " + ft_to_string(m_body.size()) + "\r\n";
    else
        header += "Content-Length: " + m_contentLength + "\r\n";
    if (m_contentType != "")
		header += "Content-Type: " + m_contentType + "\r\n";
	if (m_connection != "")
		header += "Connection: " + m_connection + "\r\n";
    if (m_code == MOVED_PERMANENTLY)
        header += "Location: http://" + m_host + ":" + m_port + m_uripath + "/" + m_query + "\r\n";
	header += "\r\n";
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
        makeErrorResponse(m_code);
    }
    else {
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

    /*
    int         hasSlash;
    // response 객체 생성될 때 파일인지 디렉토리인지 구분하는 변수 m_type 을 줘서 
    // 만약 is_exit가 true로 나와도 is_dir와 m_type이 일치하지 않으면 404에러를 내도록 

    hasSlash = 0;
    std::cout<< "checkhasSlash : " << m_requestPath[m_requestPath.size()-1] << std::endl;
    if (m_requestPath[m_requestPath.size()-1] == '/'){
        hasSlash = 1;
    }
    */

    path = m_requestPath.c_str();
    stat(path,&buf);
    is_dir = buf.st_mode & S_IFDIR;
    is_exist = access(path, F_OK); // F_OK 옵션은 파일존재여부만 확인
    // std::cout << "path : " << path << std::endl;
    // std::cout << "is_exist(-1 404) : "<< is_exist << std::endl;
    if (is_exist == -1) {
        m_code = 404;
        makeErrorResponse(m_code);
    }
    else
    {
        // std::cout <<"debug: " << hasSlash<< ", " << is_dir << std::endl;
        // if ((hasSlash == 0 && is_dir) || (hasSlash == 1 && is_dir == 0))
        // {
        //     m_code = 404;
        //     makeErrorResponse(404);
        //     return ;
        // }
        if (is_dir) // case 1 : 서빙중인 리소스가 디렉토리일 경우
        {
            if (*m_requestPath.rbegin() != '/') {
                // 요청uri가 파일을 찾고 있다면 301 리디렉션
                m_code = MOVED_PERMANENTLY;
                makeErrorResponse(m_code);
                // Location 헤더는 writeHeader() 단계에서 처리
                return;
            }
            indexHtml = m_requestPath + "/" + m_indexFile; // <- 주의 
            if (access(indexHtml.c_str(), F_OK) == 0) // 그 디렉토리에index.html이 있다면 => index.html
            {
                readFile.open(indexHtml, std::ifstream::in);
                readBuf << readFile.rdbuf();
                m_body = readBuf.str();
                readFile.close();
                m_code = 200;
                m_contentType = "text/html";
            }
            else
            {
                if (m_autoIndex == 0) //index.html이 없는데 autoindex가 off다 => 403 Forbidden
                {
                    m_code = 404;
                    makeErrorResponse(m_code);
                }
                else // index.html이 없는데 autoindex가 on이다 => 디렉토리 리스팅
                {
                    m_code = 200;
                    m_body = "";
                    makeAutoIndex();
                }    
            }
        }
        else // case 2 : 리소스가 파일인 경우
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
	    for (size_t i = 0; i < result.size(); i++){
            if (result[i].find("Status") != std::string::npos)
            {
                int start = result[i].find(" ");
                m_code = ft_stoi(result[i].substr(start, 4));
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
    m_contentLength = ft_to_string(m_body.size());
    m_body = "";
}

void Response::handlePost() {
    const char  *path;
    std::ofstream writeFile; 
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
    is_exist = access(path, F_OK);
    if (is_exist == 0){
        writeFile.open(path, std::ios_base::out);
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
    if (m_requestBody == "")
    {
        getMethod();
        return ;
    }
    if (m_cgiPath != "")
	{
        std::string retCgi = m_cgi.runCgi(m_cgiPath);
        std::vector <std::string> result; 
        result = split(retCgi, '\n');
        for (size_t i = 0; i < result.size(); i++){
            if (result[i].find("Status") != std::string::npos)
            {
                int start = result[i].find(" ");
                m_code = ft_stoi(result[i].substr(start, 4)); 
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
	    for (size_t i = 0; i < result.size(); i++){
            if (result[i].find("Status") != std::string::npos)
            {
                int start = result[i].find(" ");
                m_code = ft_stoi(result[i].substr(start, 4));
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

void			Response::deleteMethod(void) {
    struct stat buf;
    int         is_dir;
    const char  *path;
    

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
    // std::cout << ">>>>>>>> RESPONSE >>>>>>>>" << std::endl;
    // prn_prepend(m_responseMsg, ">>> ");
    // std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>\n\n" << std::endl;
    if (m_body != "")
        m_responseMsg += m_body;

}

void Response::addDirectory(std::string &body)
{
    std::string http_host_port = "http://" + m_host;
    if (m_port != "")
        http_host_port += ":" + m_port;
    if (http_host_port[http_host_port.length() - 1] != '/')
        http_host_port += "/";
    DIR *dir;
    struct dirent *diread = NULL;

    body += "<h1>Index of /autoindex/</h1><hr><br>";
    if ((dir = opendir(m_requestPath.c_str())) == NULL) 
    {
        return makeErrorResponse(500);
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

void Response::makeAutoIndex()
{
    std::string html = "";

    html += "<!DOCTYPE html>\n";
    html += "<html>\n";
    html += "<head>\n";
    html += "</head>\n";
    html += "<body>\n";
    addDirectory(html);
    html += "</body>\n";
    html += "</html>\n";
    html += "\r\n";

    m_body += html;
    m_contentType = "text/html";
}

void Response::makeErrorResponse(int error)
{
    if (error < 300)
        return ;
    if (error == 400 || error >= 500)
        m_connection = "Close";

    std::string html = "";
	html += "<!DOCTYPE html>\n";
	html += "<html>\n";
	html += "<head>\n";
	html += "</head>\n";
	html += "<body>";
	html += "<h1>" + ft_to_string(error) + " ERROR PAGE</h1>";
	html += "</body>\n";
	html += "</html>\n";

	m_body += html;
    m_contentType = "text/html";
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

