#ifndef _REQUEST_HPP_
# define _REQUEST_HPP_

# include <iostream>
# include <map>
# include <sys/time.h>
# include "utils/Uri.hpp"

# define MAX_URI 8192

enum rq_state {
	READING_STARTLINE = 0,
	READING_HEADERS,
	READING_BODY,
	READING_CHUNK_SIZE,
	READING_CHUNK_DATA,
	RECV_END
};

class Request {
	private:
		std::string m_raw;
		std::string m_body_buf;
		std::string m_method;
		Uri m_uri;
		std::string m_version;
		std::map<std::string, std::string> m_headers;
		std::string m_body;
		long m_body_length;
		unsigned long m_chunk_size;
		std::string m_chunk_data;
		int m_current_state;
		bool m_is_done;
		bool m_is_valid;
		int m_raw_idx;
		bool m_append_finished;
		int m_body_idx;
#if DEBUG
		unsigned long _elapsed;
		int _part_cnt;
#endif

		int parse_startline(std::string& buf);
		int parse_headers(std::string& buf);
		int process_startline();
		int process_headers();
		int process_body(std::string& buf);
		int process_body_chunked(std::string& buf);
		int process_body_length(std::string& buf);
		int read_chunk_size();
		int read_chunk_data();
		
	public:
		Request();
		~Request();
		int append_msg(char* buf);
		bool isDone() const;
		bool isValid() const;
		int getState() const;  // isValid()가 false면 getState()는 보내야할 응답코드를 리턴
		const std::string& getMethod() const;
		const Uri& getUri() const;
		const std::string& getVersion() const;
		std::string& getHeaderValue(const std::string& key);
		const std::string& getBody() const;
		const std::string& getRaw() const;
		
		void reset();
		const std::map<std::string,std::string>& getAllHeaders() const;
};

#endif
