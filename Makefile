NAME = a.out
SRCDIR	= srcs/
SRCFILES= ConfigServer.cpp \
          ConfigWebserv.cpp \
		  kevent_wrapper.cpp \
		  main.cpp \
		  Request.cpp \
		  Response.cpp \
		  Server.cpp \
		  utils/Uri.cpp \
		  utils/util.cpp
SRCS = $(addprefix $(SRCDIR), $(SRCFILES))

OBJS = $(SRCS:.cpp=.o)
CXX = c++
#TODO: enable -Werror -std=c++98
CXXFLAGS = -Wall -Wextra

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	SRCS += $(SRCDIR)/Webserv.cpp
else
	SRCS += $(SRCDIR)/Webserv_linux.cpp
	CXXFLAGS += -D LINUX
endif

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all
