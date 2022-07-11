#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

/*
a.out msgfile					- arg == 1: 127.0.0.1:8080에게 msgfile 전송
a.out msgfile 127.0.0.1 8080 	- arg == 3: IP:PORT에게 msgfile 전송
*/

void sendmsg(std::ifstream& infile, int fd);
void sendeee(int fd, size_t tmp_body_size);

int main(int ac, char **av) {
	std::string ip, port;
	std::ifstream infile;
	size_t tmp_body_size = 0;

	std::cout << "Usage: a.out msgfile [ip port]\n" << std::endl;
	if (ac == 2) {
		ip = "127.0.0.1";
		port = "8080";
	}
	else if (ac == 4) {
		ip = av[2];
		port = av[3];
	}
	else
		return 1;
	infile.open(av[1]);
	if (infile.fail()) {
		std::cout << "can't read " << av[1] << std::endl;
		std::cout << "instead sends 'e' " << tmp_body_size << " times." << std::endl;
		tmp_body_size = atol(av[1]);
	}
	std::cout << std::endl;
	
	int fd = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
	if (server_addr.sin_addr.s_addr == INADDR_NONE) {
		struct hostent *host = gethostbyname(ip.c_str());
		if (host && host->h_addrtype == AF_INET)
			server_addr.sin_addr = *(in_addr*)(host->h_addr);
	}
	server_addr.sin_port = htons(stoi(port));

	int ret = connect(fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (ret < 0) {
		perror("connect failed");
		exit(1);
	}
	std::cout << "connected to " << ip << ":" << port << std::endl;

	if (tmp_body_size == 0)
		sendmsg(infile, fd);
	else
		sendeee(fd, tmp_body_size);
	close(fd);
	infile.close();
	return 0;
}

void sendmsg(std::ifstream& infile, int fd) {
	std::string line;
	std::string buf;
	while (std::getline(infile, line)) {
		if (*line.rbegin() == '\r')
			line.pop_back();
		buf.append(line).append("\r\n");
	}
	std::cout << "=====   request   =====" << std::endl;
	std::cout << buf << std::endl;
	int ret = send(fd, buf.c_str(), strlen(buf.c_str()), 0);
	if (ret < 0) {
		perror("send error");
		exit(1);
	}
	std::cout << "=====   response   =====" << std::endl;
	char data[9000];
	ret = recv(fd, &data, 8999, 0);
	if (ret < 0) {
		perror("recv error");
		exit(1);
	}
	std::cout << data << std::endl;
}

void sendeee(int fd, size_t tmp_body_size) {
	std::string buf = "POST /directory/youpi.bla HTTP/1.1\r\nhost: aa\r\ncontent-length:";
	buf.append(std::to_string(tmp_body_size)).append("\r\n\r\n");

	int i = 0;
	while (i++ < tmp_body_size)
		buf.push_back('e');

	int ret = send(fd, buf.c_str(), strlen(buf.c_str()), 0);
	if (ret < 0) {
		perror("send error");
		exit(1);
	}
	std::cout << "=====   response   =====" << std::endl;
	char data[9000];
	ret = recv(fd, &data, 8999, 0);
	if (ret < 0) {
		perror("recv error");
		exit(1);
	}
	std::cout << data << std::endl;
}
