#pragma once
#include "Socket.hpp"
#include "MessageBuffer.hpp"
#include <string>

class Room;

class Client {
public:
    explicit Client(Socket socket);

    Socket& socket() { return socket_; }
    int fd() const { return socket_.fd(); }

    const std::string& username() const { return username_; }
    void setUsername(const std::string& name) { username_ = name; }
    MessageBuffer& buffer() { return buffer_; }

    Room* currentRoom() const { return currentRoom_; }
    void setCurrentRoom(Room* room) { currentRoom_ = room; }

private:
    Socket socket_;
    std::string username_;
    MessageBuffer buffer_;
    Room* currentRoom_ = nullptr;
};