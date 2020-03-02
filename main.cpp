#include <iostream>

#include "epoll.h"
#include "socket.h"

#include <thread>
#include <vector>
#include <sstream>

void IPv4TCPTest()
{
    std::vector<std::thread> threads;
    std::vector<Socket_view> sockets;
    if (auto s = IPv4Socket::CreateTCPSocket())
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
                        if(!WRITE(socket, message))
                            return;
                    }
                }
                sockets.push_back(Socket_view(*accepted));
                threads.push_back(std::thread([&sockets](IPv4Socket&& socket)
                {
                    std::cout << socket.ToStr();
                    while (true)
                    {
                        auto received = READ(socket);
                        if(!received || received->size() == 0)
                            break;
                        std::cout << *received << std::endl;
                        std::stringstream ss;
                        ss << '[' << socket.Address() << ':' << socket.Port() << "]: " << *received;
                        const auto message = ss.str();
                        for(const auto& s : sockets)
                        {
                            if (s != socket)
                            {
                                if(!WRITE(s, message))
                                    return;
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
}

void UnixTCPTest()
{
    std::vector<std::thread> threads;
    if (auto s = UnixSocket::CreateTCPSocket())
    {
        if(!s->Bind("/var/mysocket.sock"))
        {
            std::cerr << "failed to open socket" << std::endl;
            return;
        }
        s->Listen();
        std::cout << s->ToStr() << std::endl;
        if (auto accepted = s->Accept())
        {
            threads.push_back(std::thread([](UnixSocket&& socket)
            {
                std::cout << socket.ToStr();
                while (true)
                {
                    auto received = READ(socket);
                    if(!received)
                        break;
                    std::cout << *received << std::endl;
                    if(!WRITE(socket, *received))
                    {
                        break;
                    }
                }
            }, std::move(*accepted)));
        }
        else
        {
            std::cout << "failed to accept" << std::endl;
        }
    }
    for (auto& t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
}

int main()
{
    IPv4TCPTest();
    // UnixTCPTest();
    if (auto e1 = EPoll::create())
    {
        std::cout << e1->get() << std::endl;
    }
    return 0;
}
