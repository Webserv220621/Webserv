NAME = webserv
SRCDIR	= srcs/
SRCFILES= ConfigServer.cpp \
          ConfigWebserv.cpp \
		  kevent_wrapper.cpp \
		  main.cpp \
		  Request.cpp \
		  Response.cpp \
		  Server.cpp \
		  Cgi.cpp \
		  utils/Uri.cpp \
		  utils/util.cpp
SRCS = $(addprefix $(SRCDIR), $(SRCFILES))
OBJS = $(SRCS:.cpp=.o)

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98


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

debug: CXXFLAGS += -D DEBUG
debug: clean all

leaktest: CXXFLAGS += -g3 -fsanitize=address
leaktest: clean all
