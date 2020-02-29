#include "socket.h"
#include "checker.h"

#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

Socket::Socket(int descriptor, sa_family_t domain, int type, int protocol)
{
    this->fd = descriptor;
    this->type = type;
    this->domain = domain;
    this->protocol = protocol;
}

std::optional<Socket> Socket::createSocket(sa_family_t domain, int type, int protocol)
{
    //@todo: see SOCK_NONBLOCK into type parameter;
    auto fd = socket(domain, type, protocol);
    if (fd == -1)
    {
        std::cerr << strerror(errno) << std::endl;
        return {};
    }
    return std::make_optional(Socket(fd, domain, type, protocol));
}

Socket::Socket(Socket&& s)
{
    fd = s.fd;
    type = s.type;
    domain = s.domain;
    linked = s.linked;
    s.fd = -1;
}

Socket::~Socket()
{
    if (fd != -1)
    {
        if (linked && type == SOCK_STREAM)
        {
            std::cout << "shutdown" << std::endl;
            CHECK(shutdown(fd, SHUT_RDWR));
        }
        std::cout << "closing" << std::endl;
        CHECK(close(fd));
    }
}

std::optional<IPv4Socket> IPv4Socket::CreateIPv4TCPSocket()
{
    if (auto socket = createSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
    {
        return {IPv4Socket(std::move(*socket))};
    }
    return {};
}

std::optional<IPv4Socket> IPv4Socket::CreateUDPTCPSocket()
{
    if (auto socket = createSocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))
    {
        return {IPv4Socket(std::move(*socket))};
    }
    return {};
}

bool IPv4Socket::Bind(std::string_view address, uint16_t port)
{
    sockaddr_in sa = {domain,htons(port),0,0};
    auto res = CHECK_FAIL_STR(inet_pton(sa.sin_family, address.data(), &sa.sin_addr), 0, "Cannot convert to ipv4 format", 1);
    if (res)
    {
        res = CHECK(bind(fd, reinterpret_cast<sockaddr*>(&sa), sizeof(sa)));
        if (res)
        {
            params = sa;
        }
        
    }
    return res;
}

bool IPv4Socket::Bind(uint32_t address, uint16_t port)
{
    sockaddr_in sa{domain,htons(port),htonl(address),0};
    auto res = CHECK(bind(fd, reinterpret_cast<sockaddr*>(&sa), sizeof(sa)));
    if (res)
    {
        params = sa;
    }
    return res;
}

bool IPv4Socket::Listen(int maxConn)
{
    if (CHECK_STR(linked, true, "socket already connected"))
    {
        return false;
    }
    linked = CHECK(listen(fd, maxConn));
    return linked;
}

std::optional<IPv4Socket> IPv4Socket::Accept() const noexcept
{
    sockaddr_in sa;
    socklen_t socklen = 0;
    auto acceptFd = accept(fd, reinterpret_cast<sockaddr*>(&sa), &socklen);
    if (CHECK_ERROR(acceptFd))
    {
        auto socket = std::optional<IPv4Socket>(IPv4Socket(acceptFd, domain, type, protocol));
        socket->params = sa;
        socket->linked = true;
        return socket;
    }
    return {};
}

bool IPv4Socket::Connect(std::string_view address, uint16_t port)
{
    if (CHECK_STR(linked, true, "socket already connected"))
    {
        return false;
    }
    sockaddr_in sa = {domain,htons(port),0,0};
    linked = CHECK_FAIL_STR(inet_pton(sa.sin_family, address.data(), &sa.sin_addr), 0, "Cannot convert to ipv4 format", 1);
    if(linked)
    {
        linked = CHECK(connect(fd, reinterpret_cast<sockaddr*>(&sa), sizeof(sa)));
    }
    return linked;
}

bool IPv4Socket::Connect(uint32_t address, uint16_t port)
{
    if (CHECK_STR(linked, true, "socket already connected"))
    {
        return false;
    }
    sockaddr_in sa = {domain,htons(port),htonl(address),0};
    linked = CHECK(connect(fd, reinterpret_cast<sockaddr*>(&sa), sizeof(sa)));
    return linked;
}

std::string IPv4Socket::ToStr() const noexcept
{
    std::stringstream ss;
    ss << "fd: " << fd << '\n' 
        <<  "domain " << domain << '\n' 
        << "type " << type << '\n' 
        << "protocol " << protocol << '\n'
        << "is linked " << std::boolalpha << linked << '\n'
        << "address " << Address() << ":" << Port() << std::endl;
    return ss.str();
}

std::string IPv4Socket::Address() const noexcept
{
    char buf[16] = {0};
    if (!CHECK_STR(inet_ntop(domain, &params.sin_addr, buf, sizeof(buf)), nullptr, "Cannot convert ip to string"))
        return buf;
    return {};
}

uint16_t IPv4Socket::Port() const noexcept
{
    return ntohs(params.sin_port);
}

template<size_t SIZE>
inline ssize_t READ(const Socket& t, char (&buf)[SIZE])
{
    auto res = recv(t.get(), buf, sizeof(buf), MSG_NOSIGNAL);
    CHECK(res);
    return res;
}

std::optional<std::string> READ(const Socket& t)
{
    char buf[5] = {0};
    std::stringstream ss;
    while (true)
    {
        auto res = READ(t, buf);
        if (res < 0)
            return {};
        if (res == 0)
            return ss.str();
        ss << buf;
        if(buf[res - 1] == '\n')
            return ss.str();
        memset(buf,0,sizeof(buf));
    }
    
    return {};
}

bool WRITE(const Socket& t, const std::string_view msg)
{
    auto res = send(t.get(), msg.data(), msg.length(), MSG_NOSIGNAL);
    CHECK(res);
    return res < 0;
}