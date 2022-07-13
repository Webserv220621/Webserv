#ifndef _URI_HPP_
# define _URI_HPP_
# include <string>

class Uri {
	private:
		std::string m_uri;
		size_t m_cur_pos;
		std::string m_scheme;
		std::string m_host;
		std::string m_port;
		std::string m_path;
		std::string m_query;

		void parseUri();
		void setScheme();
		void setHostPort();
		void setPath();
		void setQuery();


	public:
		Uri();
		Uri(const std::string& uri);
		~Uri();
		Uri& operator=(const Uri& rhs);

		const std::string& getScheme() const;
		const std::string& getHost() const;
		const std::string& getPort() const;
		const std::string& getPath() const;
		const std::string& getQuery() const;
		size_t getLength() const;
		void reset();
};

#endif
