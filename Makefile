MAKEFLAGS	=	--no-print-directory

CXX 		?=	g++

ECHO		=	/usr/bin/echo

RM			?=	rm -f

CXXFLAGS	=	-std=c++14 -Wall -Wextra

CPPFLAGS	=	-iquote include

SRC			+=	src/Utils.cpp
SRC			+=	src/IO_Tester.cpp
SRC			+=	src/ErrorHandling.cpp
SRC			+=	src/Updater.cpp

OBJ			=	$(SRC:.cpp=.o)

NAME		=	IO_Tester

%.o:	%.cpp
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $< && \
	($(ECHO) -e "\033[92m[ OK ]\033[0m" $<) || \
	($(ECHO) -e "\033[91m[ NO COMPIL ]\033[0m" $<; false)

ifneq (,$(findstring xterm,${TERM}))
GREEN       := $(shell tput -Txterm setaf 2)
else
GREEN       := ""
endif

ifdef DEBUG
	CXXFLAGS	+=	-ggdb3
endif

all:	$(NAME)

$(NAME):	$(OBJ)
	@$(CXX) -o $(NAME) $(OBJ)

install: all
	@cp $(NAME) /usr/local/bin
	@$(ECHO) "${GREEN}[SUCCESS] Install : IO_Tester ==> /usr/local/bin"

uninstall:
	@rm /usr/local/bin/IO_Tester
	@$(ECHO) "${GREEN}[SUCCESS] Uninstall : IO_Tester"

clean:
	@for f in $(OBJ); do if [ -f $$f ]; then $(ECHO) -e "\033[91m[ RM ]\033[0m" $$f; fi; done;
	@$(RM) $(OBJ)

fclean:	clean
	@if [ -f $(NAME) ]; then $(ECHO) -e "\033[33m[ RM ]\033[0m" $(NAME); fi;
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
