#include <iostream>

#include "epoll.h"
#include "socket.h"

#include <thread>

int main()
{
    if (auto s = IPv4Socket::CreateIPv4TCPSocket())
    {
        uint16_t port = 1024;
        while(!s->Bind("127.0.0.1", port++))
        {
        }
        s->Listen();
        std::cout << s->ToStr() << std::endl;
        if (auto accepted = s->Accept())
        {
            std::cout << "info " << accepted->ToStr() << std::endl;
            char buf[100] = {0};
            READ(*accepted, buf);
            std::cout << buf << std::endl;
            WRITE(*accepted, buf);
            std::this_thread::sleep_for(std::chrono::seconds{5});
        }
        else
        {
            std::cout << "failed to accept" << std::endl;
        }
    }
    if (auto e1 = EPoll::create())
    {
        std::cout << e1->get() << std::endl;
    }
    return 0;
}
