#pragma once

#include <optional>

class EPoll
{
private:
    EPoll() = default;

public:
    static std::optional<EPoll> create(int flags = 0) noexcept;

    EPoll(EPoll&& e);

    ~EPoll();

    inline int get() const noexcept { return fd; }

private:
    int fd = -1;
};