#ifndef _RESPONSE_HPP_
# define _RESPONSE_HPP_

#include <string>
#include <map>
#include <vector>


class Response {
	private:
		// 필요한 헤더
		std::string					m_contentLength;
		std::string					m_contentType;
		std::string					m_transferEncoding;
		// key-코드, val = string 메시지, startline에 이용
		std::map<int, std::string>	m_error_msg; 
		//응답 코드 
		int							m_code;
		//응답 사이즈
		size_t						m_size;
		//최종 리턴 string
		std::string					m_responseMsg;
		//response에서 가져와야할 정보
		std::string					m_requestPath;
		std::vector<std::string> 	m_allowedMethod;  //임시 자료형
		bool						m_autoIndex;
		int							m_errCode; // 에러코드가 405 혹은 413으로 넘어오면? 

		//Server-config 에서 가져와야할 정보
		std::string m_cgiPath;
		
	public :
		//헤더, payload 작성
		std::string		getStartLine(int code);
		std::string		getHeader(void);
		std::string		getBody(void);

		//method
		void			getMethod(void);
		void			headMethod(void);
		void			postMethod(void);
		void			deleteMethod(void);

		//setter
		//getter

		//처리
		int				handlFile(void); //html파일 혹은 패스 읽기, autoindex 체크
		int				fillBody(std::string content); // 쓸 내용 m_responseMsg 작성
};
#endif