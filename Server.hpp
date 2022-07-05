#ifndef _SERVER_HPP_
# define _SERVER_HPP_

# include "common.hpp"


typedef struct location {
    // 들어가야 하는 정보 정하기 
}   loction;

enum {
    SUCCESS = 0,
    ERR_SOCKET,
    ERR_BIND,
    ERR_LISTEN,
    ERR_KQ
};

class Server 
{
    private:
        std::string m_host;
        int m_port;
        std::vector<location> locations;
        // --------- 여기까지 config data -----------
        int m_fd;  // 서버소켓

    public:
        Server();
        ~Server();
        Server(std::string host, int port); // ㅌㅔ스트용 임시 생성자
        
        int run(int kq);
        int accept_new_connection(int kq);
        int getFd() const;
};

#endif
