# include "common.hpp"
# include "Config.hpp"
# include "Server.hpp"

class Webserv 
{
    public :
        void read_config(std::string filename){
            // 파싱 함수 작성
            config.parse();
        }
        void server_run(void){
            for (int i = 0; i < 서버 개수; i++)
            {
                // 서버 개수만큼 fd = socket(),bind, listen();
            }
            server_list.run();
            kq = kqueue();
            
        }
    private :
        Config config;
        std::vector<int, Server> server_list;
        int server_len;
}