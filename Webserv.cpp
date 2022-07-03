#include <sys/event.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Webserv.hpp"

Webserv::Webserv() : m_server_cnt(0) {}
Webserv::~Webserv() {}

int Webserv::read_config(std::string filename) {
	Server s("127.0.0.1", 8000);
	m_server_list.push_back(s);
	++m_server_cnt;

	Server s2("127.0.0.1", 8080);
	m_server_list.push_back(s2);
	++m_server_cnt;

	return 0;
}

int Webserv::run() {
	int kq = kqueue();

	// 각 서버를 bind, listen
	for (int i = 0; i < m_server_cnt; ++i) {
		if (m_server_list[i].run(kq) != 0) {
			// 에러 처리
			std::cout << i + 1 << "번째 서버 구동 실패" << std::endl;
			while (i-- >= 0)
				if (m_server_list[i].getFd() != 0)
					close(m_server_list[i].getFd());
			return -1;
		}
	}
	std::cout << "listening..." << std::endl;

	if (monitor_events(kq) < 0)
		return -1;
}

int Webserv::monitor_events(int kq) {
	int MAX_EVENTS = 1000; // 임시 숫자, 해당 숫자 이상 이벤트 발생 시 다음 루프에 처리
	struct kevent eventlists[MAX_EVENTS];
	int event_count;
	while (1) {
		event_count = kevent(kq, NULL, 0, eventlists, MAX_EVENTS, NULL);
		if (event_count < 0) {
			std::cout << "kevent()에서 매우 심각한 에러" << std::endl;
			// 모든 연결 종료
			return -1;
		}
		// event_count ==0 이면 타임아웃 발생

		for (int i = 0; i < event_count; ++i) {
			int event_fd = eventlists[i].ident;
			int server_idx = getServerIdx(event_fd);
			if (server_idx >= 0) {
				std::cout << "cur server fd: " << event_fd <<std::endl;
				int connect_socket_fd = m_server_list[server_idx].accept_new_connection(kq);
				// TODO: connect_fd와 커넥션객체 연결해서 저장
			}
			else if (eventlists[i].filter & EVFILT_READ)
			{
				int rdbytes = eventlists[i].data;
				char buf[rdbytes + 1];
				int ret = recv(event_fd, &buf, rdbytes, 0);
				if (ret < 0) {
					std::cout << "recv()에서 매우 심각한 에러" << std::endl;
					return -1;
				}
				else if (ret == 0) {
					// TODO: 연결 종료
				}
				else {
					// TODO: 커넥션객체 찾아서 리퀘스트객체에게 전달
				}
			}
			else if (eventlists[i].filter & EVFILT_WRITE)
			{
				/*
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
				*/
			}
		}
		// event_count 모두 처리
		// TODO: 오래된 연결 종료
	}
}

/* fd가 서버라면 서버객체 인덱스 리턴. 서버가 아니라면 -1 리턴 */
int Webserv::getServerIdx(int fd) {
	for (int i = 0; i < m_server_cnt; i++) {
		if (fd == m_server_list[i].getFd())
			return i;
	}
	return -1;
}








int main() {
	Webserv webserv;
	
	if (webserv.read_config("") < 0) {
		std::cout << "read_config failed." << std::endl;
	}

	webserv.run();
}
