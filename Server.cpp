#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/event.h>
#include "Server.hpp"
#include "kevent_wrapper.hpp"

Server::Server() : m_fd(0) {}
Server::~Server() {}
Server::Server(std::string host, int port) : m_fd(0), m_host(host), m_port(port) {}

/* 서버소켓을 만들고 bind, listen 한 다음 kqueue에 서버소켓 추가
 * 문제 없으면 0 리턴
 * 소켓 생성 실패하면 1, bind 실패하면 2, listen 실패하면 3 리턴 */
int Server::run(int kq) {
	m_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (m_fd < 0)
		return ERR_SOCKET;
	
	fcntl(m_fd, F_SETFL, O_NONBLOCK);
	int option = 1;
	setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(m_host.c_str());
	server_addr.sin_port = htons(m_port);
	
	int ret = bind(m_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (ret < 0)
		return ERR_BIND;

	ret = listen(m_fd, 100); //서버 소켓은 closed 상태에서 listen상태로 변경, 두 번째 인자는 연결요청 대기 큐의 크기
	if (ret < 0)
		return ERR_LISTEN;

	ret = add_read_filter(kq, m_fd);
	if (ret < 0)
		return ERR_KQ;
	
	return SUCCESS;
}

/* 연결요청을 accept()하고 새 커넥션소켓을 kqueue에 추가.
 * 커넥션 소켓을 리턴 */
int Server::accept_new_connection(int kq) {
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);

	int conn_fd = accept(m_fd, (struct sockaddr*)&client_addr, &client_addr_len);
	if (conn_fd > 0) {
		fcntl(conn_fd, F_SETFL, O_NONBLOCK);
		add_read_filter(kq, conn_fd);
	}
	return conn_fd;
}

int Server::getFd() const {
	return m_fd;
}
