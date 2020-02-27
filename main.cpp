#include <iostream>

#include "epoll.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>
#include <optional>
#include <arpa/inet.h>

class Socket
{
private:
    Socket() = default;

public:
    static std::optional<Socket> createIPv4TCPSocket()
    {
        //@todo: see SOCK_NONBLOCK into type parameter;
        auto fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

    bool bindIPv4(std::string address, uint16_t port)
    {
        struct sockaddr_in sa = {
            domain,
            htons(port),
            0,
            0
        };

        switch (inet_pton(sa.sin_family, address.c_str(), &sa.sin_addr))
        {
        case 1:
            break;
        case 0:
            std::cerr << "cannot convert to ipv4 format" << std::endl;
            return false;
        default:
            std::cerr << strerror(errno) << std::endl;
            return false;
        }

        if (bind(fd, reinterpret_cast<sockaddr*>(&sa), sizeof(sa)) < 0)
        {
            std::cerr << strerror(errno) << std::endl;
            return false;
        }

        return true;
    }

    bool bindIPv4(uint32_t address, uint16_t port)
    {
        struct sockaddr_in sa = {
            domain,
            htons(port),
            htonl(address),
            0
        };

        if (bind(fd, reinterpret_cast<sockaddr*>(&sa), sizeof(sa)) < 0)
        {
            std::cerr << strerror(errno) << std::endl;
            return false;
        }

        return true;
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
    sa_family_t domain;
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
