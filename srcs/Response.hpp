#ifndef _RESPONSE_HPP_
# define _RESPONSE_HPP_

#include "Config.hpp"
#include "Request.hpp"

class Response {
	private:
		std::string 				m_raw_string;
		std::string 				m_requestPath; // 매핑된 filepath
		size_t						m_bodySize;		// contentlength

		// header
		std::string					m_contentType;	//header
		std::string					m_contentLength; // header
		std::string					m_connection; // 필요시 closed 
		//
		int							m_code;
		bool						m_autoIndex; // 필요시 html 작성
		std::string					m_responseMsg; // req 메시지 전체
		std::string					m_body;
		std::string 				m_cgiPath;
		std::string					m_method; // req 객체로부터 가져올?
		std::map<int, std::string>  m_errorMsg;


	public:
		Response();
		~Response() {};
		void 			initResponse(void);
		int 			validCheck(void);
		void 			runResponse(void);
		std::string 	writeBody(void);
		void			writeResponseMsg(void);
		void			handleGet(void);

		//method
		void			getMethod(void);
		void			headMethod(void);
		void			postMethod(void);
		void			deleteMethod(void);
		// getter
		std::string		getStartLine(void);
		std::string		getHeader(void);
		int			 	getCode(void);
		std::string		getBody(void);
		// setter
		void setPath(std::string path);
		//
		std::string getRawString() { return m_raw_string; };
		void setRawString(std::string ss) { m_raw_string = ss;}
};

#endif