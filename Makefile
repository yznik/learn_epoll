all: server

server: main.o epoll.o
	g++ -Wall -Werror -Wextra -std=c++17 -o server main.o epoll.o

main.o:
	g++ -Wall -Werror -Wextra -std=c++17 -c main.cpp

epoll.o:
	g++ -Wall -Werror -Wextra -std=c++17 -c epoll.cpp

clean:
	@rm -rf server *.o