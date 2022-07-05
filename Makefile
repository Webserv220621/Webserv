NAME = a.out
SRCS = srcs/ConfigServer.cpp \
	   srcs/ConfigWebserv.cpp \
	   srcs/kevent_wrapper.cpp \
	   srcs/main.cpp \
	   srcs/Request.cpp \
	   srcs/Server.cpp \
	   srcs/Util.cpp \
	   srcs/Webserv.cpp

OBJS = $(SRCS:.cpp=.o)
CXX = c++
CXXFLAGS = -Wall -Wextra

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all
