#pragma once
#include "Socket.hpp"
#include "Client.hpp"
#include "RoomManager.hpp"
#include <vector>
#include <memory>
#include <poll.h>

class Server {
public:
    bool run(uint16_t port);

private:
    Socket listener_;
    std::vector<pollfd> fds_;
    std::vector<std::unique_ptr<Client>> clients_;
    RoomManager roomManager_;

    void acceptNewClient();
    void removeClient(size_t fdsIndex);
    void handleClientData(size_t fdsIndex);
    void sendHelp(Client& client);
    Client* findClientByUsername(const std::string& username);
};