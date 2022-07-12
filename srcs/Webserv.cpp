#include <sys/event.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Config.hpp"
#include "Connection.hpp"
#include "kevent_wrapper.hpp"
#include "common.hpp"
#include "utils/util.hpp"

int Webserv::run() {
	int kq = kqueue();

	int ret;
	// 각 서버를 bind, listen
	for (int i = 0; i < m_server_cnt; ++i) {
		ret = m_server_list[i].run(kq);
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

	if (monitor_events(kq) < 0)
		return -1;
	// ===== May Not Reachable =====
	for (int i = 0; i < m_server_cnt; ++i)
		close(m_server_list[i].getFd());
	return 0;
}

int Webserv::monitor_events(int kq) {
	std::map<int, Connection> connection_list;
	/* fd로부터 Connection객체를 찾기 위해..
	 * map으로 관리할 때 장점: 구현 간단, 메모리 낭비x
	 * vector로 관리할 때 장점: 빠른 탐색 */


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
				int connect_socket_fd = m_server_list[server_idx].accept_new_connection(kq);
				if (connect_socket_fd < 0) {
					std::cout << "can't accept new connection request" << std::endl;
					continue;
				}
				// std::cout << "new connection on server " << event_fd - 3 << std::endl;
				// connect_fd와 커넥션객체 연결해서 저장
				connection_list[connect_socket_fd].fd = connect_socket_fd;
				connection_list[connect_socket_fd].server = m_server_list[server_idx];
			}
			else if (eventlists[i].filter == EVFILT_READ)
			{
				int rdbytes = eventlists[i].data;
				char buf[rdbytes + 1];
				int ret = recv(event_fd, &buf, rdbytes, 0);
				if (ret < 0) {
					std::cout << "recv()에서 매우 심각한 에러" << std::endl;
					return -1;
				}
				else if (ret == 0) {
					// 연결 종료
					close(event_fd);
					connection_list.erase(event_fd);
					// std::cout << "connection closed" << std::endl;
				}
				else {
					// 커넥션객체 찾아서 리퀘스트객체에게 전달
					buf[rdbytes] = '\0';
					Request& rq = connection_list[event_fd].request;
					int result = rq.append_msg(buf);
					if (! rq.isDone())
						continue;
					// std::cout << "<<<<<<<< REQUEST <<<<<<<<" << std::endl;
					// prn_prepend(rq.getRaw(), "<<< ");
					// std::cout << "<<< body size=" << rq.getBody().size() << std::endl;
					// std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
					// 수신 완료됐으면 리스폰스 메시지 생성
					Response& resp = connection_list[event_fd].response;
					resp.initResponse(connection_list[event_fd].server, connection_list[event_fd].request);
					resp.runResponse();
					// kqueue 에서 C-R 삭제, C-W 추가
					remove_read_filter(kq, event_fd);
					add_write_filter(kq, event_fd);
				}
			}
			else if (eventlists[i].filter == EVFILT_WRITE)
			{
				Response& response = connection_list[event_fd].response;
				const std::string& str = response.getResponseMsg();
				size_t sent_bytes = response.getSentBytes();
				int ret;

				size_t remain_bytes = str.length() - sent_bytes;
				ret = send(event_fd, str.substr(sent_bytes, remain_bytes).c_str(), remain_bytes, 0);
				if (ret <= 0) {
					std::cout << "send()에서 매우 심각한 에러" << std::endl;
					return -1;
				}
				if (ret != remain_bytes) {
					// std::cout << "sent " << ret << " bytes" << std::endl;
					response.setSentBytes(sent_bytes + ret);
				}

				if (sent_bytes + ret >= str.length()) {
					// 전송 완료. C-W 삭제.
					// 연결유지할거면 리퀘스트객체 초기화 후 C-R 추가.
// TODO: needs test on MacOS
#if 1
					remove_write_filter(kq, event_fd);
					if (response.isKeepAlive()) {
						connection_list[event_fd].request.reset();
						add_read_filter(kq, event_fd);
					}
					else {
						connection_list.erase(event_fd);
						close(event_fd);
						std::cout << "connection closed" << std::endl;
					}
#else
					connection_list.erase(event_fd);
					close(event_fd);
					// std::cout << "connection closed" << std::endl;
#endif
				}
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
