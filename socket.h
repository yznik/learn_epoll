#pragma once

#include "checker.h"

#include <optional>
#include <arpa/inet.h>

class Socket
{
public:
    Socket(Socket&& s);

    ~Socket();

    inline int get() const noexcept { return fd; }

protected:
    Socket(int descriptor, sa_family_t domain, int type, int protocol);

    static std::optional<Socket> createSocket(sa_family_t domain, int type, int protocol);

protected:
    int fd = -1;
    int type;
    sa_family_t domain;
    int protocol;
    bool linked = false;
};

class IPv4Socket : public Socket
{
public:
    IPv4Socket(IPv4Socket&& s): Socket(std::move(s))
    {
        params = s.params;
    }

    static std::optional<IPv4Socket> CreateIPv4TCPSocket();
    static std::optional<IPv4Socket> CreateUDPTCPSocket();

    bool Bind(std::string_view address, uint16_t port);
    bool Bind(uint32_t address, uint16_t port);

    bool Listen(int maxConn = SOMAXCONN);
    std::optional<IPv4Socket> Accept() const noexcept;

    bool Connect(std::string_view address, uint16_t port);
    bool Connect(uint32_t address, uint16_t port);

    std::string ToStr() const noexcept;

private:
    using Socket::Socket;
    IPv4Socket(Socket&& s): Socket(std::move(s)) {}

private:
    sockaddr_in params;
};

template<size_t SIZE>
void READ(const Socket& t, char (&buf)[SIZE])
{
    recv(t.get(), buf, sizeof(buf), MSG_NOSIGNAL);
}

template<size_t SIZE>
void WRITE(const Socket& t, char (&buf)[SIZE])
{
    send(t.get(), buf, sizeof(buf), MSG_NOSIGNAL);
}
