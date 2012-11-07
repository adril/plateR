##
## Makefile
##

CXX      = g++
CXXFLAGS += -W -Wall -Wextra -g3
LDFLAGS  +=
LFLAGS = -lboost_system

NAME  = server
SRC   = main.cpp			\
	client.cpp			\
	server.cpp

OBJ   = $(SRC:.cpp=.o)

all : $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) *~

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
