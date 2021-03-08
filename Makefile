MAKEFLAGS	=	--no-print-directory

CXX 		?=	g++

RM			?=	rm -f

CXXFLAGS	=	-std=c++14 -Wall -Wextra

CPPFLAGS	=	-iquote include

SRC			+=	src/Utils.cpp
SRC			+=	src/IO_Tester.cpp
SRC			+=	src/ErrorHandling.cpp
SRC			+=	src/Diff.cpp
SRC			+=	src/Updater.cpp

OBJ			=	$(SRC:.cpp=.o)

NAME		=	IO_Tester

ifneq (,$(findstring xterm,${TERM}))
GREEN       := $(shell tput -Txterm setaf 2)
RED         := $(shell tput -Txterm setaf 1)
RESET 		:= $(shell tput -Txterm sgr0)
else
GREEN       := ""
RED         := ""
RESET       := ""
endif

%.o:	%.cpp
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $< && \
	(echo "${GREEN}[OK]${RESET}" $<) || \
	(echo "${RED}[BUILD ERROR]${RESET}" $<; false)

ifdef DEBUG
	CXXFLAGS	+=	-ggdb3
endif

all:	$(NAME)

$(NAME):	$(OBJ)
	@$(CXX) -o $(NAME) $(OBJ)

install: all
	@cp $(NAME) /usr/local/bin
	@echo "${GREEN}[SUCCESS]${RESET} Install : IO_Tester ==> /usr/local/bin"

uninstall:
	@rm /usr/local/bin/IO_Tester
	@echo "${GREEN}[SUCCESS]${RESET} Uninstall : IO_Tester"

clean:
	@for f in $(OBJ); do if [ -f $$f ]; then echo "${RED}[RM]${RESET}" $$f; fi; done;
	@$(RM) $(OBJ)

fclean:	clean
	@if [ -f $(NAME) ]; then echo "${RED}[RM]${RESET}" $(NAME); fi;
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
