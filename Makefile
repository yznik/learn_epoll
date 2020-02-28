all: server

server: main.o epoll.o socket.o
	g++ -Wall -Werror -Wextra -std=c++17 -o server main.o epoll.o socket.o

main.o:
	g++ -Wall -Werror -Wextra -std=c++17 -c main.cpp

epoll.o:
	g++ -Wall -Werror -Wextra -std=c++17 -c epoll.cpp

socket.o:
	g++ -Wall -Werror -Wextra -std=c++17 -c socket.cpp

clean:
	@rm -rf server *.o