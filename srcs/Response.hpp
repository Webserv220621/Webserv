#ifndef _RESPONSE_HPP_
# define _RESPONSE_HPP_

#include "Config.hpp"
#include "Request.hpp"
#include "Cgi.hpp"

class Response {
	private:

		std::string 				m_requestPath; // 매핑된 filepath
		std::string 				m_requestBody; // 있다면
		size_t						m_bodySize;		// contentlength

		// header
		std::string					m_contentType;	//header
		std::string					m_contentLength; // header
		std::string					m_connection; // 필요시 closed 
		//
		int							m_code;
		bool						m_autoIndex; // 필요시 html 작성
		std::string					m_responseMsg; // resp 메시지 전체
		size_t						m_sent_bytes; // 어디까지 보냈는지 기억할 변수
		std::string					m_body;
		std::string 				m_cgiPath;
		std::string					m_method; // req 객체로부터 가져올?
		static std::map<int, std::string>  m_errorMsg;
		std::string					m_indexFile; // location에서 가져오는 index default 파일


		//
		std::string					m_host;
		std::string					m_port;
		Location					m_location;

		Location		findMatchingLocation(Server& server, Request& rq);

		Cgi							m_cgi;

	public:
		Response();
		~Response() {};
		void 			initResponse(Server& server, Request& request);
		int 			validCheck(void);
		void 			runResponse(void);
		void			handleGet(void);
		void			handlePost(void);
		void			handlePut(void);

		//method
		void			getMethod(void);
		void			headMethod(void);
		void			postMethod(void);
		void			deleteMethod(void);
		void 			putMethod(void);

		// getter
		std::string		getResponseMsg(void);
		int			 	getCode(void);
		size_t			getSentBytes(void);
		bool			isKeepAlive();

		// setter
		size_t			setSentBytes(size_t n);
		std::string		writeStartLine(void);
		std::string		writeHeader(void);
		std::string 	writeBody(void);
		void			writeResponseMsg(void);




		void setPath(std::string path);	// test용 임시 
		void			setMethod(std::string method); // test용 임시 
		void			setBody(std::string body);
		//

		void makeErrorResponse(int error);// error경우
		void makeAutoIndex();//autoindex == 1 경우
		void addDirectory(std::string &body); // directory 출력
};

#endif
