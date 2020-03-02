all: server

server: main.o epoll.o socket.o
	g++ -ggdb -Wall -Werror -Wextra -std=c++17 -o server main.o epoll.o socket.o -pthread

main.o:
	g++ -ggdb -Wall -Werror -Wextra -std=c++17 -c main.cpp

epoll.o:
	g++ -ggdb -Wall -Werror -Wextra -std=c++17 -c epoll.cpp

socket.o:
	g++ -ggdb -Wall -Werror -Wextra -std=c++17 -c socket.cpp

clean:
	@rm -rf server *.o
