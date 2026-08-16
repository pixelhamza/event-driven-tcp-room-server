#include "Server.hpp"
#include "Protocol.hpp"
#include <iostream>
#include <utility>

bool Server::run(uint16_t port) {
    if (!listener_.listenOn(port)) {
        std::cerr << "Failed to start server\n";
        return false;
    }
    std::cout << "Server listening on port " << port << "...\n";

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

        for (size_t i = fds_.size(); i-- > 1;) {
            short events = fds_[i].revents;

            if (events & (POLLHUP | POLLERR | POLLNVAL)) {
                removeClient(i);
                continue;
            }

            if (events & POLLIN) {
                handleClientData(i);
            }
        }
    }

    return true;
}

void Server::acceptNewClient() {
    Socket clientSocket = listener_.accept();
    if (clientSocket.isValid()) {
        std::cout << "New client connected (fd=" << clientSocket.fd() << ")\n";

        pollfd clientPfd;
        clientPfd.fd = clientSocket.fd();
        clientPfd.events = POLLIN;
        clientPfd.revents = 0;
        fds_.push_back(clientPfd);

        auto clientPtr = std::make_unique<Client>(std::move(clientSocket));
        Client* rawPtr = clientPtr.get();
        clients_.push_back(std::move(clientPtr));

        // Move newly connected client into default #lobby room
        roomManager_.moveClient(rawPtr, "#lobby");

        // Send welcome notice & command help
        std::string welcome = "=== Welcome to TCP Room Chat Server! ===\n"
                              "You joined #lobby. Type /help for available commands.\n";
        rawPtr->socket().sendAll(welcome.c_str(), welcome.size());
    }
}

void Server::removeClient(size_t fdsIndex) {
    Client* clientPtr = clients_[fdsIndex - 1].get();

    std::cout << "Client " << clientPtr->username() << " (fd=" << clientPtr->fd() << ") disconnected\n";

    // Cleanly remove from current room before destroying client
    roomManager_.removeClient(clientPtr);

    fds_.erase(fds_.begin() + fdsIndex);
    clients_.erase(clients_.begin() + (fdsIndex - 1));
}

Client* Server::findClientByUsername(const std::string& username) {
    for (auto& clientPtr : clients_) {
        if (clientPtr->username() == username) {
            return clientPtr.get();
        }
    }
    return nullptr;
}

void Server::handleClientData(size_t fdsIndex) {
    Client& sender = *clients_[fdsIndex - 1];
    char buffer[1024];
    int bytesRead = sender.socket().recv(buffer, sizeof(buffer) - 1);

    if (bytesRead <= 0) {
        removeClient(fdsIndex);
        return;
    }

    sender.buffer().append(std::string_view(buffer, bytesRead));

    auto& msgBuffer = sender.buffer();

    while (auto message = msgBuffer.nextMessage()) {
        Command cmd = Protocol::parse(*message);

        switch (cmd.type) {
            case CommandType::NICK: {
                if (!cmd.arg.empty()) {
                    std::string oldName = sender.username();
                    sender.setUsername(cmd.arg);

                    std::string notice = "*** " + oldName + " is now known as " + cmd.arg + " ***\n";
                    if (sender.currentRoom()) {
                        sender.currentRoom()->sendSystemNotice(notice);
                    }
                }
                break;
            }

            case CommandType::JOIN: {
                if (!cmd.arg.empty()) {
                    roomManager_.moveClient(&sender, cmd.arg);
                } else {
                    std::string err = "Usage: /join <room_name>\n";
                    sender.socket().sendAll(err.c_str(), err.size());
                }
                break;
            }

            case CommandType::LEAVE: {
                roomManager_.moveClient(&sender, "#lobby");
                break;
            }

            case CommandType::ROOMS: {
                std::string roomList = roomManager_.listRooms();
                sender.socket().sendAll(roomList.c_str(), roomList.size());
                break;
            }

            case CommandType::USERS: {
                if (sender.currentRoom()) {
                    std::string userList = sender.currentRoom()->getMemberList();
                    sender.socket().sendAll(userList.c_str(), userList.size());
                }
                break;
            }

            case CommandType::MSG: {
                if (cmd.arg.empty()) {
                    std::string err = "Usage: /msg <username> <message>\n";
                    sender.socket().sendAll(err.c_str(), err.size());
                    break;
                }

                size_t spacePos = cmd.arg.find(' ');
                if (spacePos == std::string::npos) {
                    std::string err = "Usage: /msg <username> <message>\n";
                    sender.socket().sendAll(err.c_str(), err.size());
                    break;
                }

                std::string targetName = cmd.arg.substr(0, spacePos);
                size_t msgStart = cmd.arg.find_first_not_of(' ', spacePos);
                if (msgStart == std::string::npos) {
                    std::string err = "Usage: /msg <username> <message>\n";
                    sender.socket().sendAll(err.c_str(), err.size());
                    break;
                }
                std::string pmBody = cmd.arg.substr(msgStart);

                Client* recipient = findClientByUsername(targetName);
                if (!recipient) {
                    std::string err = "*** User '" + targetName + "' not found ***\n";
                    sender.socket().sendAll(err.c_str(), err.size());
                } else if (recipient == &sender) {
                    std::string err = "*** You cannot send a private message to yourself ***\n";
                    sender.socket().sendAll(err.c_str(), err.size());
                } else {
                    std::string incomingPm = "[PM from " + sender.username() + "]: " + pmBody + "\n";
                    recipient->socket().sendAll(incomingPm.c_str(), incomingPm.size());

                    std::string outgoingPm = "[PM to " + recipient->username() + "]: " + pmBody + "\n";
                    sender.socket().sendAll(outgoingPm.c_str(), outgoingPm.size());
                }
                break;
            }

            case CommandType::HELP: {
                sendHelp(sender);
                break;
            }

            case CommandType::CHAT_MESSAGE: {
                if (sender.currentRoom()) {
                    std::string outgoing = sender.username() + ": " + cmd.rawCommand + "\n";
                    std::cout << "[" << sender.currentRoom()->name() << "] " << outgoing;
                    sender.currentRoom()->broadcast(outgoing, &sender);
                }
                break;
            }

            case CommandType::UNKNOWN:
            default: {
                std::string err = "Unknown command. Type /help for options.\n";
                sender.socket().sendAll(err.c_str(), err.size());
                break;
            }
        }
    }
}

void Server::sendHelp(Client& client) {
    std::string help = "=== Available Commands ===\n"
                       "  /nick <name>          - Change your username\n"
                       "  /join <room>          - Join or create a room (e.g. /join #tech)\n"
                       "  /leave                - Return to default #lobby\n"
                       "  /rooms                - List all active rooms\n"
                       "  /users                - List users in your current room\n"
                       "  /msg <user> <msg>     - Send a private message to a user\n"
                       "  /help                 - Show this help message\n";
    client.socket().sendAll(help.c_str(), help.size());
}