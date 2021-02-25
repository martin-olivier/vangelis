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
	@cp $(NAME) /usr/local/bin
	@echo "\033[92m[INSTALL] Success : IO_Tester ==> /usr/local/bin\033[0m"

clean:
	$(RM) $(OBJ)

fclean:	clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
