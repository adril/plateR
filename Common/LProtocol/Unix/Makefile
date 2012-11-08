NAME		= 	libProtocol.so

SRC		= 	srcs/LProtocol.cpp\
			srcs/Message.cpp\
			srcs/MessageLogin.cpp\
			srcs/MessageLoginResult.cpp\
			srcs/MessageLogOut.cpp\
			srcs/MessagePlate.cpp\
			srcs/MessageFile.cpp


INC		=	-I. -I../include/ -I../../User/

OBJ		=	$(SRC:.cpp=.o)
CXX		=	g++
CXXFLAGS	=       -fPIC -O3 -W -Wall -Werror
CXXDEBUG        =       -g3 -ggdb
RM		=	rm -Rf
ECHO		=	echo -e

$(NAME)		:	$(OBJ)
			$(CXX) -shared -rdynamic $(INC) $(CXXFLAGS) -o $(NAME) $(OBJ)
			cp -f $(NAME) ../../
			@$(ECHO) '\033[0;32m> $(NAME) compiled\033[0m'

clean		:
			-@$(RM) $(OBJ)
			-@$(RM) *~
			-@$(RM) \#*\#
			@$(ECHO) '\033[0;35m> Directory cleaned\033[0m'

all		:	$(NAME)

fclean		:	clean
			-@$(RM) $(NAME)
			@$(ECHO) '\033[0;35m> Remove $(NAME)\033[0m'

re		:	fclean all

debug		:	$(OBJ)
			@$(CXX) -shared -rdynamic $(INC) $(CXXDEBUG) -o $(NAME) $(OBJ)
			cp -f $(NAME) ../../
			@$(ECHO) '\033[0;32m> Mode Debug: done\033[0m'

.PHONY		:	all clean re fclean debug

.cpp.o		:	
			$(CXX) $(INC) $(CXXFLAGS) -o $@ -c $<
