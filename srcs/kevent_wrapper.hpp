#ifndef _KEVENT_WRAPPER_HPP_
# define _KEVENT_WRAPPER_HPP_

int add_read_filter(int kq, int fd);
int remove_read_filter(int kq, int fd);
int add_write_filter(int kq, int fd);

#endif
