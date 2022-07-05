#ifndef _WEBSERV_HPP_
# define _WEBSERV_HPP_

# include <vector>
# include "Server.hpp"

class Webserv 
{
    private:
        //Config config;
        int m_server_cnt;
        std::vector<Server> m_server_list;

        int getServerIdx(int fd);

    public:
        Webserv();
        ~Webserv();
        int read_config(std::string filename);
        int run();
        int monitor_events(int kq);
};

#endif
