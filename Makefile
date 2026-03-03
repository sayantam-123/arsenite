.PHONY: clean all

all: parser

parser: parser.cpp lexer.h lexer.cpp
	g++ -Wall -Wextra -Wswitch -ggdb parser.cpp -o parser -I./

clean:
	rm -rf parser
