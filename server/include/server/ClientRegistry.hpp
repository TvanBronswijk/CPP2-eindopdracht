#pragma once
#include <thread>
#include "ClientInfo.hpp"

namespace server {
    class Server;
    class ClientRegistry {
    public:
        ClientRegistry() = default;
        ~ClientRegistry();
        void register_client(Server&, connection::Socket);

        size_t size() { return _clients.size(); }
        std::vector<std::weak_ptr<ClientInfo>>::iterator begin() { return _clients.begin(); }
        std::vector<std::weak_ptr<ClientInfo>>::iterator end() { return _clients.end(); }
        std::weak_ptr<ClientInfo> operator[] (size_t index) { return _clients.at(index); }

    private:
        std::vector<std::thread> _threads;
        std::vector<std::weak_ptr<ClientInfo>> _clients;

        friend void client_thread(Server&, connection::Socket);
    };
}