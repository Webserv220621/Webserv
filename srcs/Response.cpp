#include "Response.hpp"

Response::Response () {
    initResponse();
}

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

bool Response::validUrl(void) {
    if (m_requestPath == "404")
        return 0;
    return 1;
}

void Response::runResponse () {

    if (validUrl == 0)
    {
        m_code = 404;
        writeBody();
    }
    else{
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
void			Response::getMethod(void) {

}

void			Response::headMethod(void) {

}

void			Response::postMethod(void) {

}

void			Response::deleteMethod(void) {

}

std::string Response::writeBody () {

}

void Response::writeResponseMsg(void) {
    m_responseMsg += getStartLine();
    m_responseMsg += getHeader();
    m_responseMsg += writeBody();
}
