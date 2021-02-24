MAKEFLAGS	=	--no-print-directory

CXX 		?=	g++

RM			?=	rm -f

CXXFLAGS	=	-std=c++14 -Wall -Wextra

CPPFLAGS	=	-iquote include

SRC			+=	src/Utils.cpp
SRC			+=	src/IO_Tester.cpp
SRC			+=	src/ErrorHandling.cpp

OBJ			=	$(SRC:.cpp=.o)

NAME		=	IO_Tester

ifdef DEBUG
	CXXFLAGS	+=	-ggdb3
endif

all:	$(NAME)

$(NAME):	$(OBJ)
	$(CXX) -o $(NAME) $(OBJ)
	$(RM) $(OBJ)

install: all
	@echo -e "\033[92mCopying IO_Tester into /usr/bin\033[0m"
	@sudo cp $(NAME) /usr/bin

user_install: all
	@echo -e "\033[92mCopying IO_Tester into ${HOME}/.local/bin\033[0m"
	@cp $(NAME) ${HOME}/.local/bin

clean:
	$(RM) $(OBJ)

fclean:	clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
