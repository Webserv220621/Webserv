#include "Uri.hpp"

Uri::Uri() {}

Uri::Uri(const std::string& uri) : m_uri(uri), m_cur_pos(0) {
	parseUri();
}

Uri::~Uri() {}

Uri& Uri::operator=(const Uri& rhs) {
	m_uri = rhs.m_uri;
	m_cur_pos = rhs.m_cur_pos;
	m_scheme = rhs.m_scheme;
	m_host = rhs.m_host;
	m_port = rhs.m_port;
	m_path = rhs.m_path;
	m_query = rhs.m_query;
	return *this;
}

void Uri::parseUri() {
	// "://" 를 찾아서 앞부분은 sheme에 저장, cur_pos는 뒷부분을 가리키도록 업데이트
	setScheme();
	// '/'를 찾아서 앞부분을 host와 port로 나눠 저장
	setHostPort();
	// '?'를 찾아서 앞부분을 path로 저장
	setPath();
	// 나머지 중 '#' 앞부분까지가 query
	setQuery();
}

void Uri::setScheme() {
	size_t found_pos = m_uri.find("://");
	if (found_pos == std::string::npos)
		m_scheme = "";
	else {
		m_scheme = m_uri.substr(0, found_pos);
		m_cur_pos = found_pos + 3;
	}
}

void Uri::setHostPort() {
	size_t found_pos = m_uri.find('/', m_cur_pos);

	// 현재 문자열이 /로 시작한다면 host,port 정보 없음
	if (found_pos == 0)
		m_host = m_port = "";
	else {
		// '/'가 존재하지 않는다면 문자열 끝에 있는걸로 취급
		if (found_pos == std::string::npos)
			found_pos = m_uri.size();
		
		// host와 port 분리
		size_t port_pos = m_uri.find(':', m_cur_pos);
		if (port_pos == std::string::npos || port_pos > found_pos) {
			m_host = m_uri.substr(m_cur_pos, found_pos - m_cur_pos);
			m_port = "";
		}
		else {
			m_host = m_uri.substr(m_cur_pos, port_pos - m_cur_pos);
			m_port = m_uri.substr(port_pos + 1, found_pos - port_pos - 1);
		}
		m_cur_pos = found_pos;
	}
}

void Uri::setPath() {
	size_t found_pos = m_uri.find('?', m_cur_pos);
	if (found_pos == std::string::npos) {
		found_pos = m_uri.size();
		m_path = m_uri.substr(m_cur_pos, found_pos - m_cur_pos);
		m_cur_pos = found_pos;
	}
	else {
		m_path = m_uri.substr(m_cur_pos, found_pos - m_cur_pos);
		m_cur_pos = found_pos + 1;
	}

}

void Uri::setQuery() {
	m_query = m_uri.substr(m_cur_pos);
}


// =========== getter =============
const std::string& Uri::getScheme() const {
	return m_scheme;
}

const std::string& Uri::getHost() const {
	return m_host;
}

const std::string& Uri::getPort() const {
	return m_port;
}

const std::string& Uri::getPath() const {
	return m_path;
}

const std::string& Uri::getQuery() const {
	return m_query;
}

const std::string& Uri::getRaw() const {
	return m_uri;
}

size_t Uri::getLength() const {
	return m_uri.length();
}

void Uri::reset() {
	m_uri = m_scheme = m_host = m_port = m_path = m_query = "";
	m_cur_pos = 0;
}
