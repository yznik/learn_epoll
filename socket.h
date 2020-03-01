#pragma once

#include "checker.h"

#include <optional>
#include <string>

#include <arpa/inet.h>

class Socket
{
public:
    Socket(Socket&& s);
    Socket& operator=(Socket&&) = delete;
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

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

class Socket_view
{
public:
    Socket_view(int descriptor): fd(descriptor) {}
    inline int get() const noexcept { return fd; }
private:
    int fd = -1;
};

inline bool operator==(const Socket& s, const Socket_view& sv)
{
    return s.get() == sv.get();
}

inline bool operator==(const Socket_view& sv, const Socket& s)
{
    return s == sv;
}

class IPv4Socket : public Socket
{
public:
    IPv4Socket(IPv4Socket&& s): Socket(std::move(s))
    {
        params = s.params;
    }
    IPv4Socket& operator=(IPv4Socket&& s) = delete;
    IPv4Socket(const IPv4Socket&) = delete;
    IPv4Socket& operator=(const IPv4Socket&) = delete;

    static std::optional<IPv4Socket> CreateIPv4TCPSocket();
    static std::optional<IPv4Socket> CreateUDPTCPSocket();

    bool Bind(std::string_view address, uint16_t port);
    bool Bind(uint32_t address, uint16_t port);

    bool Listen(int maxConn = SOMAXCONN);
    std::optional<IPv4Socket> Accept() const noexcept;

    bool Connect(std::string_view address, uint16_t port);
    bool Connect(uint32_t address, uint16_t port);

    std::string ToStr() const noexcept;

    std::string Address() const noexcept;
    uint16_t Port() const noexcept;

private:
    using Socket::Socket;
    explicit IPv4Socket(Socket&& s): Socket(std::move(s)) {}

private:
    sockaddr_in params;
};

std::optional<std::string> READ(const Socket& t);
bool WRITE(const Socket& t, const std::string_view msg);
