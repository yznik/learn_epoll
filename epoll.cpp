#include "epoll.h"

#include <iostream>

#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>

EPoll::EPoll(EPoll&& e)
{
    fd = e.fd;
    e.fd = -1;
}

EPoll::~EPoll()
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

std::optional<EPoll> EPoll::create(int flags) noexcept
{
    auto fd = epoll_create1(flags);
    if (fd == -1)
    {
        std::cerr << strerror(errno) << std::endl;
        return {};
    }
    EPoll e;
    e.fd = fd;
    return std::make_optional<EPoll>(std::move(e));
}
