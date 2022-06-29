# include "common.hpp"
# include "Config.hpp"
# include "Server.hpp"

class Webserv 
{
    public :
        void read_config(std::string filename){
            config.parse();
        }
        void server_run(void){
            for (i = 0; i < 서버 개수; i++)
            {
                
            }
            server_list.run();
        }
    private :
        Config config;
        std::vector<int, Server> server_list;
        int server_len;
}