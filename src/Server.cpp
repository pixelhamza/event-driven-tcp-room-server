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

        clients_.push_back(Client(std::move(client)));
    }
}

void Server::handleClientData(size_t fdsIndex) {
    char buffer[1024];
    int bytesRead = clients_[fdsIndex - 1].socket().recv(buffer, sizeof(buffer) - 1);

    if (bytesRead <= 0) {
        std::cout << "Client fd=" << clients_[fdsIndex - 1].fd() << " disconnected\n";
        fds_.erase(fds_.begin() + fdsIndex);
        clients_.erase(clients_.begin() + (fdsIndex - 1));
        return;
    }

    buffer[bytesRead] = '\0';
    std::string message(buffer);

    while (!message.empty() && (message.back() == '\n' || message.back() == '\r')) {
        message.pop_back();
    }

    Client& sender = clients_[fdsIndex - 1];

    const std::string nickPrefix = "/nick ";
    if (message.rfind(nickPrefix, 0) == 0) { 
        std::string newName = message.substr(nickPrefix.size());
        if (!newName.empty()) {
            std::string oldName = sender.username();
            sender.setUsername(newName);

            std::string notice = oldName + " is now known as " + newName + "\n";
            std::cout << notice;
            for (auto& c : clients_) {
                c.socket().sendAll(notice.c_str(), notice.size());
            }
        }
        return;
    }

    std::string outgoing = sender.username() + ": " + message + "\n";
    std::cout << outgoing;
    for (size_t j = 0; j < clients_.size(); ++j) {
        if (j == fdsIndex - 1) continue;
        clients_[j].socket().sendAll(outgoing.c_str(), outgoing.size());
    }
}