#include <iostream>
#include <algorithm>
#include "Request.hpp"
#include "common.hpp"
#include "utils/util.hpp"

Request::Request() {
	reset();
}

Request::~Request() {};

int Request::append_msg(char* str) {
#if DEBUG
	struct timeval _start_time;
	gettimeofday(&_start_time, NULL);
	if (_part_cnt == 0) {
		__LOG("< receiving request msg...");
	}
	_part_cnt++;
#endif
	int ret = 0;
	const std::string& m_body_buf = str;
	if (m_current_state <= READING_HEADERS)
		m_raw.append(str);
	else
		m_body_raw.append(str);
	std::string buf = "";

	m_append_finished = false;
	while (!m_append_finished) {
		if (m_current_state == READING_STARTLINE)
			ret = process_startline();
		else if (m_current_state == READING_HEADERS)
			ret = process_headers();
		else if (m_current_state == READING_BODY || m_current_state == READING_CHUNK_DATA || m_current_state == READING_CHUNK_SIZE)
			ret = process_body(buf);
		else
			break;
		if (ret != 0) {
			m_current_state = RECV_END;
			m_is_done = true;
			m_is_valid = false;
#if DEBUG
			struct timeval current;
			gettimeofday(&current, NULL);
			_elapsed += (current.tv_sec - _start_time.tv_sec) * 1000 * 1000 + (current.tv_usec - _start_time.tv_usec);
			std::cout << "   [ invalid request --- elapsed: " << (_elapsed / 1000) << "ms ]" << std::endl;
#endif
			return ret;
		}
	}

#if DEBUG
	struct timeval current;
	gettimeofday(&current, NULL);
	_elapsed += (current.tv_sec - _start_time.tv_sec) * 1000 * 1000 + (current.tv_usec - _start_time.tv_usec);
	if (m_is_done)
		std::cout << "   [ request part: " << _part_cnt << ",  elapsed: " << (_elapsed / 1000) << "ms ]" << std::endl;
#endif

	return ret;
}

// line을 공백단위로 잘라서 method, uri, version에 저장
int Request::parse_startline(std::string& line) {
	size_t start_pos = 0, next;

	next = line.find(' ');
	if (next == std::string::npos)
		return BAD_REQUEST;
	m_method = line.substr(0, next);
	start_pos = next;
	while (line[start_pos] == ' ')
		start_pos++;
	next = line.find(' ', start_pos);
	if (next == std::string::npos)
		return BAD_REQUEST;
	m_uri = Uri(line.substr(start_pos, next - start_pos));
	if (m_uri.getLength() > MAX_URI)
		return URI_TOO_LONG;
	if (m_uri.getHost() != "" && m_uri.getScheme() != "http")
		return BAD_REQUEST;
	if (m_uri.getPath()[0] != '/')
		return BAD_REQUEST;
	start_pos = next;
	while (line[start_pos] == ' ')
		start_pos++;
	m_version = line.substr(start_pos);
	if (m_version.substr(0,5) != "HTTP/")
		return BAD_REQUEST;
	if ( ! (m_version.substr(5) == "1.0" || m_version.substr(5) == "1.1") )
		return HTTP_VERSION_NOT_SUPPORTED;
	return 0;
}

// line을 colon단위로 잘라서 key:value로 저장
int Request::parse_headers(std::string& line) {
	size_t pos = line.find(':');
	std::string key = line.substr(0, pos);
	key = trimSpace(key);
	std::transform(key.begin(), key.end(), key.begin(), ::tolower);
	std::string value = line.substr(pos+1);
	value = trimSpace(value);
	std::transform(value.begin(), value.end(), value.begin(), ::tolower);
	if (key == "host" && m_headers.count("host"))
		return BAD_REQUEST;
	if (key == "transfer-encoding" && value != "chunked")
		return NOT_IMPLEMENTED;
	m_headers[key] = value;
	return 0;
}

int Request::process_startline() {
	size_t n;
	
	n = m_raw.find("\r\n", m_raw_idx);
	if (n == std::string::npos) {
		m_append_finished = true;
		return 0;
	}

	std::string line = m_raw.substr(0, n);
	m_raw_idx = n + 2;
	int ret = parse_startline(line);
	if (ret != 0)
		return ret;
	else
		m_current_state = READING_HEADERS;
	return 0;
}

int Request::process_headers() {
	size_t n;

	n = m_raw.find("\r\n", m_raw_idx);
	if (n == std::string::npos) {
		m_append_finished = true;
		return 0;
	}
	if (n == m_raw_idx) {       // empty line. header수신 완료
		m_body_idx = n;
		if (m_headers.count("host") == 0)
			return BAD_REQUEST;

		m_is_done = true;
		m_is_valid = true;
		m_current_state = RECV_END;

		if ( m_headers.count("content-length") || m_headers.count("transfer-encoding") ) {
			m_raw_idx = n + 2;
			if (m_headers.count("content-length")) {
				m_body_length = strtol(m_headers["content-length"].c_str(), NULL, 0);
				if (m_body_length < 0)
					return BAD_REQUEST;
			}
			if (m_raw_idx < m_raw.size()) {
				m_body_buf = m_raw.substr(m_raw_idx);
				m_raw_idx = 0;
			}
			else {
				m_body_buf = "";
				m_raw_idx = 0;
			}
			m_current_state = READING_BODY;
			m_is_done = false;
			if (m_headers.count("transfer-encoding"))
				m_current_state = READING_CHUNK_SIZE;
		}
	}
	else {
		std::string line = m_raw.substr(m_raw_idx, n - m_raw_idx);
		m_raw_idx = n + 2;
		return parse_headers(line);
	}
	return 0;
}

int Request::process_body(std::string& buf) {
	if (m_current_state == READING_BODY)
		return process_body_length(buf);
	else if (m_current_state == READING_CHUNK_SIZE)
		return read_chunk_size();
	else
		return read_chunk_data();
}

int Request::process_body_length(std::string& buf) {
	if (m_raw.size() - m_raw_idx >= static_cast<size_t>(m_body_length)) {
		m_body = m_raw.substr(m_raw_idx);
		m_is_done = true;
		m_is_valid = true;
		m_current_state = RECV_END;
	}
	else
		m_append_finished = true;
	return 0;
}

int Request::read_chunk_size() {
	// 1. 16진수 H<CR><LF> 읽기
	size_t n = m_raw.find("\r\n", m_raw_idx);
	if (n == std::string::npos) {
		m_append_finished = true;
		return 0;
	}

	std::string tmp = m_raw.substr(m_raw_idx, m_raw_idx - n);
	// chunk size를 16진수->10진수로 변환
	m_chunk_size = strtol(tmp.c_str(), NULL, 16);
	m_current_state = READING_CHUNK_DATA;
//	m_current_body_size += m_chunk_size;
	m_raw_idx = n + 2;
//	m_chunk_received_size = 0;
	m_chunk_data = "";
	return 0;
}

int Request::read_chunk_data() {
	// chunk_data가 m_chunk_size + \r\n을 가지고 있을 때까지 대기
	if (m_raw_idx + m_chunk_size + 2 <= m_raw.size()) {
		if (m_chunk_size == 0) {
			m_is_done = true;
			m_is_valid = true;
			m_current_state = RECV_END;
			return 0;
		}
		std::string tmp = m_raw.substr(m_raw_idx, m_chunk_size);
		m_body.append(tmp);
		m_raw_idx = m_raw_idx + m_chunk_size + 2;
		m_current_state = READING_CHUNK_SIZE;
	}
	else
		m_append_finished = true;
	return 0;
}


bool Request::isDone() const {
	return m_is_done;
}

bool Request::isValid() const {
	return m_is_valid;
}

int Request::getState() const {
	return m_current_state;
}
const std::string& Request::getMethod() const {
	return m_method;
}
const Uri& Request::getUri() const {
	return m_uri;
}
const std::string& Request::getVersion() const {
	return m_version;
}

std::string& Request::getHeaderValue(const std::string& key) {
	return m_headers[key];
}

const std::string& Request::getBody() const {
	return m_body;
}

const std::string& Request::getRaw() const {
	return m_raw;
}

void Request::reset() {
	m_raw = "";
	m_method = "";
	m_uri.reset();
	m_version = "";
	m_headers.clear();
	m_body = "";
	m_body_length = 0;
	m_chunk_size = 0;
	m_chunk_data = "";
	m_current_state = READING_STARTLINE;
	m_is_done = false;
	m_is_valid = false;
	m_raw_idx = 0;
	m_body_idx = 0;
#if DEBUG
	_elapsed = 0;
	_part_cnt = 0;
#endif
}

const std::map<std::string,std::string>& Request::getAllHeaders() const {
	return m_headers;
}
