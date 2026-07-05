#include "Server.hpp"
#include <iostream>
#include <utility>

bool Server::run(uint16_t port) {
    if (!listener_.listenOn(port)) {
        std::cerr << "Failed to start server\n";
        return false;
    }
    std::cout << "We listening on " << port << std::endl;

    pollfd listenerPfd;
    listenerPfd.fd = listener_.fd();
    listenerPfd.events = POLLIN;
    listenerPfd.revents = 0;
    fds_.push_back(listenerPfd);

    while (true) {
        int ready = poll(fds_.data(), fds_.size(), -1);
        if (ready < 0) {
            std::cerr << "poll failed\n";
            break;
        }

        if (fds_[0].revents & POLLIN) {
            acceptNewClient();
        }

        for (size_t i = fds_.size(); i-- > 1; ) {
            if (fds_[i].revents & POLLIN) {
                handleClientData(i);
            }
        }
    }

    return true;
}

void Server::acceptNewClient() {
    Socket client = listener_.accept();
    if (client.isValid()) {
        std::cout << "New Connection established" << std::endl;

        pollfd clientPfd;
        clientPfd.fd = client.fd();
        clientPfd.events = POLLIN;
        clientPfd.revents = 0;
        fds_.push_back(clientPfd);

        clients_.push_back(std::move(client));
    }
}

void Server::handleClientData(size_t fdsIndex) {
    char buffer[1024];
    int bytesRead = clients_[fdsIndex - 1].recv(buffer, sizeof(buffer) - 1);

    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::cout << " Message by fd " << fdsIndex << ": " << buffer;

        for (size_t j = 0; j < clients_.size(); ++j) {
            if (j == fdsIndex - 1) continue;
            clients_[j].sendAll(buffer, bytesRead);
        }
    } else {
        std::cout << "Client at fds[" << fdsIndex << "] disconnected\n";
        fds_.erase(fds_.begin() + fdsIndex);
        clients_.erase(clients_.begin() + (fdsIndex - 1));
    }
}