#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <sys/event.h>
#include <fcntl.h>
#include <string.h>
#include <vector>
#include <ctime>


void run_kqueue(int kq, std::vector<int> server_fds);

struct Server {
		const char *ip;
		const int port;
	};

void server_run(void) {
	Server s1 = {"127.0.0.1", 1200};
	Server s2 = {"127.0.0.1", 1202};
	int server_len = 2;
	std::vector<Server> v;
	v.push_back(s1);
	v.push_back(s2);
	std::vector<int> server_fds;
	for (int i = 0; i < server_len; i++){
		int server_fd = socket(PF_INET, SOCK_STREAM, 0);
		if (server_fd < 0) {
			perror("socket error");
			exit(1);
		}
		fcntl(server_fd, F_SETFL, O_NONBLOCK);
		int option = 1;
		setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
		struct sockaddr_in server_addr;
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = inet_addr(v[i].ip);  // Webserv-config에서 가져옴
		server_addr.sin_port = htons(v[i].port); // Webserv-config에서 가져옴
		int ret = bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
		if (ret < 0) {
			std::cerr << server_fd << server_addr.sin_addr.s_addr << server_addr.sin_port << std::endl;
			// 에러처리 
			perror("bind error");
			exit(1);
		}
		ret = listen(server_fd, 100); //서버 소켓은 closed 상태에서 listen상태로 변경, 두 번째 인자는 연결요청 대기 큐의 크기 
		if (ret < 0) {
			perror("listen error");
			exit(1);
		}
		server_fds.push_back(server_fd);
	}
	int kq = kqueue();
	run_kqueue(kq, server_fds);
}

void run_kqueue(int kq, std::vector<int> server_fds){
	struct timespec time_set;
	time_set.tv_nsec = 0;
	time_set.tv_sec = 2;
	for (int i = 0; i < server_fds.size(); i++){
		struct kevent change_list;
		EV_SET(&change_list, server_fds[i], EVFILT_READ, EV_ADD | EV_ENABLE, NULL, NULL, NULL);
		int ret = kevent(kq, &change_list, 1, NULL, 0, &time_set);
		if (ret < 0) {
			perror("kevent fail");
			exit(1);
		}
	}
	std::cout << server_fds[0] << server_fds[1] <<std::endl;
	int MAX_EVENTS = 1000; // 임시 숫자, 해당 숫자 이상 이벤트 발생 시 다음 루프에 처리
	struct kevent eventlists[MAX_EVENTS];
	int event_count;
	while (1) {
		event_count = kevent(kq, NULL, 0, eventlists, MAX_EVENTS, &time_set);
		if (event_count < 0) {
			std::cout << strerror(errno) << std::endl;
			exit(1);
		}
		for (int i = 0; i < event_count; i++) {
			int event_fd = eventlists[i].ident;
			if (find(server_fds.begin(), server_fds.end(), event_fd) != server_fds.end())
			{
				std::cout << "cur server fd: " << event_fd <<std::endl;
				int connect_socket_fd;
				int server_fd = event_fd;
				struct sockaddr_in client_addr;
				socklen_t client_addr_len = sizeof(client_addr);
				connect_socket_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
				fcntl(connect_socket_fd, F_SETFL, O_NONBLOCK);
				struct kevent changelist_connection;
				EV_SET(&changelist_connection, connect_socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, NULL, NULL, NULL);
				kevent(kq, &changelist_connection, 1, NULL, 0, &time_set);
			}
			else if (eventlists[i].filter & EVFILT_READ)
			{
				int connect_socket_fd = event_fd;
				int rdbytes = eventlists[i].data;
				int ret;
				if (rdbytes > 0) {
					char buf[4096];
					ret = recv(connect_socket_fd, &buf, rdbytes, 0);
					if (ret <0)
					{
						close(connect_socket_fd);
						perror("data_rev func fail");
						exit(1);
					}
					buf[rdbytes] = '\0';
					std::cout << "received: " << buf << std::endl; // 수정 필요 
					write(connect_socket_fd, buf, rdbytes); // 수정 필요 -> 커넥션 객체의 Request buf에 담아줘야 함
				}
				else
				{ 
					std::cout << connect_socket_fd << " disconnected" << std::endl;
					close(connect_socket_fd);
				}
			}
			else if (eventlists[i].filter & EVFILT_WRITE)
			{
				int connect_socket_fd = event_fd;
				int sent_data = 0;
				// Connection 객체 전체 사이즈 = RCV_SIZE
				std::string	str = 커넥션객체.보낼데이터.substr(sent_data, RECV_SIZE);
				int	ret = ::send(connect_socket_fd, str.c_str(), str.size(), 0);
				if (ret == -1)
				{
					close(connect_socket_fd);
					perror("data_send func fail");
					exit(1);
				}
				else
				{
					sent_data += ret;
					if (sent_data >= RCV_SIZE)
						close(connect_socket_fd);
				}
			}
		}
	}
	close(kq);
	for (int i = 0; i < server_fds.size(); i++)
		close(server_fds[i]);
}

int main(void){
	server_run();
	return (0);
}