#pragma once
#include <thread>
#include "ClientInfo.hpp"

class Server;
struct ClientRegistry {
    std::vector<std::thread> threads;
    std::vector<std::shared_ptr<ClientInfo>> clients;

    void register_client(Server&, Socket);
    std::vector<std::shared_ptr<ClientInfo>>::iterator begin() { return clients.begin(); }
    std::vector<std::shared_ptr<ClientInfo>>::iterator end() { return clients.end(); }

};