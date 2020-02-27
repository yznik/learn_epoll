#include <iostream>

#include "epoll.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>
#include <optional>

class Socket
{
private:
    Socket() = default;

public:
    static std::optional<Socket> createIPv4TCPSocket()
    {
        //@todo: see SOCK_NONBLOCK into type parameter;
        auto fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1)
        {
            std::cout << -1 << std::endl;
            std::cerr << strerror(errno) << std::endl;
            return {};
        }
        Socket s;
        s.fd = fd;
        s.type = SOCK_STREAM;
        s.domain = AF_INET;
        return std::make_optional(std::move(s));
    }
    Socket(Socket&& s)
    {
        fd = s.fd;
        type = s.type;
        s.fd = -1;
    }
    ~Socket()
    {
        if (fd != -1)
        {
            if (connected && type == SOCK_STREAM)
            {
                if (shutdown(fd, SHUT_RDWR) == -1)
                {
                    std::cerr << strerror(errno) << std::endl;
                }
            }
            close(fd);
        }
    }

    inline int get() const noexcept { return fd; }
private:
    int fd = -1;
    int type;
    int domain;
    bool connected = false;
};

int main()
{
    if (auto s = Socket::createIPv4TCPSocket())
    {
        std::cout << s->get() << std::endl;
    }
    if (auto e1 = EPoll::create())
    {
        std::cout << e1->get() << std::endl;
    }
    return 0;
}
