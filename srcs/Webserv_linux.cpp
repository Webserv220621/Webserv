#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Config.hpp"
#include "Connection.hpp"
#include "kevent_wrapper.hpp"
#include "common.hpp"

int Webserv::run() {
	int epoll_fd = epoll_create(1024);

	int ret;
	// 각 서버를 bind, listen
	for (int i = 0; i < m_server_cnt; ++i) {
		ret = m_server_list[i].run(epoll_fd);
		if (ret != SUCCESS) {
			if (ret == ERR_SOCKET)
				std::cout << "cannot create socket for server " << i + 1 << std::endl;
			else if (ret == ERR_BIND)
				std::cout << "cannot bind address of server " << i + 1 << std::endl;
			else if (ret == ERR_LISTEN)
				std::cout << "server " << i + 1 << " cannot listen for connections" << std::endl;
			else if (ret == ERR_KQ)
				std::cout << "can't add server " << i + 1 << " to kqueue" << std::endl;

			while (i-- >= 0)
				if (m_server_list[i].getFd() != 0)
					close(m_server_list[i].getFd());
			return -1;
		}
	}
	for (int i = 0; i < m_server_cnt; ++i)
		std::cout << "server " << i + 1 << ": " << m_server_list[i].getHost() << ":" << m_server_list[i].getPort() << std::endl;
	std::cout << "listening..." << std::endl;

	if (monitor_events(epoll_fd) < 0)
		return -1;
	// ===== May Not Reachable =====
	for (int i = 0; i < m_server_cnt; ++i)
		close(m_server_list[i].getFd());
	return 0;
}

int Webserv::monitor_events(int epoll_fd) {
	std::map<int, Connection> connection_list;
	/* fd로부터 Connection객체를 찾기 위해..
	 * map으로 관리할 때 장점: 구현 간단, 메모리 낭비x
	 * vector로 관리할 때 장점: 빠른 탐색 */


	int MAX_EVENTS = 1000; // 임시 숫자, 해당 숫자 이상 이벤트 발생 시 다음 루프에 처리
	struct epoll_event eventlists[MAX_EVENTS];
	int event_count;
	while (1) {
		event_count = epoll_wait(epoll_fd, eventlists, MAX_EVENTS, -1);
		if (event_count < 0) {
			std::cout << "kevent()에서 매우 심각한 에러" << std::endl;
			// 모든 연결 종료
			return -1;
		}
		// event_count ==0 이면 타임아웃 발생

		for (int i = 0; i < event_count; ++i) {
			int event_fd = eventlists[i].data.fd;
			int server_idx = getServerIdx(event_fd);
			std::cout << server_idx;
			if (server_idx >= 0) {
				std::cout << "cur server fd: " << event_fd <<std::endl;
				int connect_socket_fd = m_server_list[server_idx].accept_new_connection(epoll_fd);
				// connect_fd와 커넥션객체 연결해서 저장
				connection_list[connect_socket_fd].fd = connect_socket_fd;
				connection_list[connect_socket_fd].server = m_server_list[server_idx];
			}
			else if (eventlists[i].events == EPOLLIN)
			{
				int rdbytes = 4096;
				char buf[rdbytes];
				int ret = recv(event_fd, &buf, rdbytes, 0);
				if (ret < 0) {
					std::cout << "recv()에서 매우 심각한 에러" << std::endl;
					return -1;
				}
				else if (ret == 0) {
					// 연결 종료
					close(event_fd);
					connection_list.erase(event_fd);
				}
				else {
					// 커넥션객체 찾아서 리퀘스트객체에게 전달
					buf[ret] = '\0';
					Request& rq = connection_list[event_fd].request;
					int result = rq.append_msg(buf);
					if (! rq.isDone())
						continue;
					// 수신 완료됐으면 리스폰스 메시지 생성
					Response& resp = connection_list[event_fd].response;
					resp.initResponse(connection_list[event_fd].server, connection_list[event_fd].request);
					resp.runResponse();
					// kqueue 에서 C-R 삭제, C-W 추가
					remove_read_filter(epoll_fd, event_fd);
					add_write_filter(epoll_fd, event_fd);
				}
			}
			else if (eventlists[i].events == EPOLLOUT)
			{
				Response& response = connection_list[event_fd].response;
				std::string str = response.getResponseMsg();
				// TODO: 나눠보내기
				send(event_fd, str.c_str(), str.size(), 0);
				connection_list.erase(event_fd);
				close(event_fd);
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
