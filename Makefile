ifneq (,$(findstring xterm,${TERM}))
GREEN       := $(shell tput -Txterm setaf 2)
RED         := $(shell tput -Txterm setaf 1)
RESET 		:= $(shell tput -Txterm sgr0)
else
GREEN       := ""
RED         := ""
RESET       := ""
endif

all:
	cmake . -B build/ -DCMAKE_BUILD_TYPE=Debug
	cmake --build build/

tests:
	cmake . -B build_tests/ -DCMAKE_BUILD_TYPE=Debug -DUNIT_TESTS=ON
	cmake --build build_tests/

install: all
	@cp IO_Tester /usr/local/bin
	@echo "${GREEN}[SUCCESS]${RESET} IO_Tester has been installed at /usr/local/bin"

uninstall:
	@rm /usr/local/bin/IO_Tester
	@echo "${GREEN}[SUCCESS]${RESET} IO_Tester has been unistalled"

clean:
	@rm -rf build
	@rm -rf build_tests

fclean:	clean
	@rm -rf IO_Tester
	@rm -rf unit_tests

.PHONY: all tests clean fclean install uninstall