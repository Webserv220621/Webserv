#include <iostream>
#include "Request.hpp"

#define FAIL -1

Request::Request() {
	m_current_state = READING_STARTLINE;
	m_is_done = false;
	m_is_valid = false;
}

Request::~Request() {};

int Request::append(std::string& buf) {
	
	while (!buf.empty()) {
		if (m_current_state == READING_STARTLINE)
			process_startline(buf);
		else if (m_current_state == READING_HEADERS)
			process_headers(buf);
		else if (m_current_state == READING_BODY)
			process_body(buf);
		else
			break;
	}
	return 0;
}

// line을 공백단위로 잘라서 method, uri, version에 저장
int Request::parse_startline(std::string& line) {
	size_t start_pos = 0, next;
	next = line.find(' ');
	m_method = line.substr(0, next);
	start_pos = next;
	while (line[start_pos] == ' ')
		start_pos++;
	next = line.find(' ', start_pos);
	m_uri = line.substr(start_pos, next - start_pos);
	start_pos = next;
	while (line[start_pos] == ' ')
		start_pos++;
	m_version = line.substr(start_pos);
	return 0;
}

// line을 colon단위로 잘라서 key:value로 저장
int Request::parse_headers(std::string& line) {
	// TODO: 앞뒤공백자르기, 대소문자구분x
	size_t pos = line.find(':');
	std::string key = line.substr(0, pos);
	std::string value = line.substr(pos+1);
	m_headers[key] = value;
	return 0;
}

int Request::process_startline(std::string& buf) {
	m_prev.append(buf);

	size_t n = m_prev.find("\r\n");
	if (n == std::string::npos) {
		buf = "";
	}
	else {
		std::string line = m_prev.substr(0, n);
		buf = m_prev.substr(n + 2);
		m_prev = "";
		if (parse_startline(line) == FAIL) {
			m_is_done = true;
			m_is_valid = false;
		}
		else {
			m_current_state = READING_HEADERS;
		}
	}
	return 0;
}

int Request::process_headers(std::string& buf) {
	m_prev.append(buf);
	size_t n = m_prev.find("\r\n");
	if (n == std::string::npos) {
		buf = "";
	}
	else if (n == 0) {	// empty line. header수신 완료
		if (m_method == "GET" || m_method == "HEAD" || m_method == "DELETE") {
			m_is_done = true;
			m_is_valid = true;
			m_current_state = RECV_END;
		}
		else if (m_method == "POST") {
			if (m_headers["content-length"].empty() && m_headers["transfer-encoding"].empty()) {
				m_is_done = true;
				m_is_valid = false;
				m_current_state = RECV_END;
			}
			else {
				m_current_state = READING_BODY;
				buf = m_prev.substr(n+2);
				m_prev = "";
				if (!m_headers["transfer-encoding"].empty())
					m_body_chunked = true;
				else
					m_body_length = strtol(m_headers["content-length"].c_str(), NULL, 0);
			}
		}
		else
			;
	}
	else {
		std::string line = m_prev.substr(0, n);
		buf = m_prev.substr(n+2);
		m_prev = "";
		if (parse_headers(line) == FAIL) {
			m_is_done = true;
			m_is_valid = false;
		}
	}
	return 0;
}
int Request::process_body(std::string& buf) {
	m_prev.append(buf);
	if (m_body_chunked)
		return process_body_chunked(buf);
	else
		return process_body_length(buf);

}

int Request::process_body_chunked(std::string& buf) {
	buf = "";
	return 0;
}

int Request::process_body_length(std::string& buf) {
	if (m_prev.size() >= m_body_length) {
		m_body = m_prev.substr(0, m_body_length);
		m_is_done = true;
		m_is_valid = true;
		m_current_state = RECV_END;
	}
	else
		buf = "";
	return 0;
}


const int Request::getState() const {
	return m_current_state;
}
const std::string& Request::getMethod() const {
	return m_method;
}
const std::string& Request::getUri() const {
	return m_uri;
}
const std::string& Request::getVersion() const {
	return m_version;
}

std::string& Request::getHeaderValue(std::string& key) {
	return m_headers[key];
}

const std::map<std::string,std::string>& Request::getAllHeaders() const {
	return m_headers;
}

const std::string& Request::getBody() const {
	return m_body;
}

bool Request::isValid() const {
	return m_is_valid;
}
