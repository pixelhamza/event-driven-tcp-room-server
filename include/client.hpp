#pragma once
#include "Socket.hpp"
#include <string>

class Client {
public:
    explicit Client(Socket socket);

    Socket& socket() { return socket_; }
    int fd() const { return socket_.fd(); }

    const std::string& username() const { return username_; }
    void setUsername(const std::string& name) { username_ = name; }

private:
    Socket socket_;
    std::string username_;
};