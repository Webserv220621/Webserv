#include <sys/event.h>
#include "kevent_wrapper.hpp"

#define NULL 0

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
