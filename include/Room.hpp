#pragma once
#include <string>
#include <unordered_set>
#include <mutex>
#include "Client.hpp"

class Room {
public:
    explicit Room(std::string name);

    const std::string& name() const { return name_; }

    void addClient(Client* client);
    void removeClient(Client* client);
    bool hasClient(Client* client) const;

    void broadcast(const std::string& message, Client* sender = nullptr);

    void sendSystemNotice(const std::string& notice);

    std::string getMemberList() const;

    size_t memberCount() const;
    bool isEmpty() const;

private:
    std::string name_;
    std::unordered_set<Client*> members_;
    mutable std::mutex mutex_;
};
