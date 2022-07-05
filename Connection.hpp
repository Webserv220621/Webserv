#ifndef _CONNECTION_HPP_
# define _CONNECTION_HPP_

# include "Request.hpp"
# include "Response.hpp"

struct Connection {
    int fd;
    Server server;
    Request request;
    Response response;
};

#endif
