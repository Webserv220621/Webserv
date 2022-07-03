#ifndef _RESPONSE_HPP_
# define _RESPONSE_HPP_

class Response {
	private:
		std::string m_raw_string;

	public:
		std::string getRawString() { return m_raw_string; };
		void setRawString(std::string ss) { m_raw_string = ss;}
};

#endif
