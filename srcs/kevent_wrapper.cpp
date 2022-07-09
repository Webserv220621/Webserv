#ifndef LINUX
	#include <sys/event.h>
	#include "kevent_wrapper.hpp"

	#define NULL 0
#else
	#include <sys/epoll.h>
	#include <cstring> /// memset
#endif

#ifndef LINUX
int add_read_filter(int kq, int fd) {
	struct kevent tmp;
	EV_SET(&tmp, fd, EVFILT_READ, EV_ADD | EV_ENABLE, NULL, NULL, NULL);
	return kevent(kq, &tmp, 1, NULL, 0, NULL);
}

int remove_read_filter(int kq, int fd) {
	struct kevent tmp;
	EV_SET(&tmp, fd, EVFILT_READ, EV_DELETE, NULL, NULL, NULL);
	return kevent(kq, &tmp, 1, NULL, 0, NULL);
}

int add_write_filter(int kq, int fd) {
	struct kevent tmp;
	EV_SET(&tmp, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, NULL, NULL, NULL);
	return kevent(kq, &tmp, 1, NULL, 0, NULL);
}

int remove_write_filter(int kq, int fd) {
	struct kevent tmp;
	EV_SET(&tmp, fd, EVFILT_WRITE, EV_DELETE, NULL, NULL, NULL);
	return kevent(kq, &tmp, 1, NULL, 0, NULL);
}

#else
int add_read_filter(int kq, int fd) {
	struct epoll_event tmp;
	memset(&tmp, 0, sizeof(tmp));
	tmp.events = EPOLLIN;
	tmp.data.fd = fd;
	return epoll_ctl(kq, EPOLL_CTL_ADD, fd, &tmp);
}

int remove_read_filter(int kq, int fd) {
	struct epoll_event tmp;
	memset(&tmp, 0, sizeof(tmp));
	tmp.events = EPOLLOUT;
	tmp.data.fd = fd;
	return epoll_ctl(kq, EPOLL_CTL_MOD, fd, &tmp);
}

int add_write_filter(int kq, int fd) {
	struct epoll_event tmp;
	memset(&tmp, 0, sizeof(tmp));
	tmp.events = EPOLLOUT;
	tmp.data.fd = fd;
	return epoll_ctl(kq, EPOLL_CTL_ADD, fd, &tmp);
}

int remove_write_filter(int kq, int fd) {
	struct epoll_event tmp;
	memset(&tmp, 0, sizeof(tmp));
	tmp.events = EPOLLIN;
	tmp.data.fd = fd;
	return epoll_ctl(kq, EPOLL_CTL_MOD, fd, &tmp);
}

#endif
