#include <iostream>

#include "epoll.h"
#include "socket.h"

#include <thread>
#include <vector>
#include <sstream>

int main()
{
    std::vector<std::thread> threads;
    std::vector<IPv4Socket*> sockets;
    if (auto s = IPv4Socket::CreateIPv4TCPSocket())
    {
        int connections = 2;
        uint16_t port = 1024;
        while(!s->Bind("127.0.0.1", port++))
        {
        }
        s->Listen();
        std::cout << s->ToStr() << std::endl;
        while (true)
        {
            if (auto accepted = s->Accept())
            {
                {
                    std::stringstream ss;
                    ss << "Client " << accepted->Address() << ':' << accepted->Port() << " connected" << std::endl;
                    const auto message = ss.str();
                    for(const auto& socket: sockets)
                    {
                        WRITE(*socket, message);
                    }
                }
                threads.push_back(std::thread([&sockets](IPv4Socket&& socket)
                {
                    std::cout << socket.ToStr();
                    sockets.push_back(&socket);
                    while (true)
                    {
                        auto received = READ(socket);
                        if(!received)
                            break;
                        std::cout << *received << std::endl;
                        std::stringstream ss;
                        ss << '[' << socket.Address() << ':' << socket.Port() << "]: " << *received;
                        const auto message = ss.str();
                        for(const auto& s : sockets)
                        {
                            if (s->get() != socket.get())
                            {
                                WRITE(*s, message);
                            }
                        }
                    }
                }, std::move(*accepted)));
                connections--;
            }
            else
            {
                std::cout << "failed to accept" << std::endl;
            }
            if (connections == 0)
                break;
        }
    }
    for (auto& t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
    if (auto e1 = EPoll::create())
    {
        std::cout << e1->get() << std::endl;
    }
    return 0;
}
