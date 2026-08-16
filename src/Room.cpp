#include "Room.hpp"
#include <sstream>
#include <iostream>
#include <utility>

Room::Room(std::string name) : name_(std::move(name)) {}

void Room::addClient(Client* client) {
    if (!client) return;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        members_.insert(client);
    }

    std::string notice = "*** " + client->username() + " joined " + name_ + " ***\n";
    sendSystemNotice(notice);
}

void Room::removeClient(Client* client) {
    if (!client) return;

    bool wasMember = false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        wasMember = (members_.erase(client) > 0);
    }

    if (wasMember) {
        std::string notice = "*** " + client->username() + " left " + name_ + " ***\n";
        sendSystemNotice(notice);
    }
}

bool Room::hasClient(Client* client) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return members_.find(client) != members_.end();
}

void Room::broadcast(const std::string& message, Client* sender) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (Client* client : members_) {
        if (client != sender) {
            client->socket().sendAll(message.c_str(), message.size());
        }
    }
}

void Room::sendSystemNotice(const std::string& notice) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (Client* client : members_) {
        client->socket().sendAll(notice.c_str(), notice.size());
    }
}

std::string Room::getMemberList() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;
    oss << "Users in " << name_ << " (" << members_.size() << "):\n";
    for (Client* client : members_) {
        oss << " - " << client->username() << "\n";
    }
    return oss.str();
}

size_t Room::memberCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return members_.size();
}

bool Room::isEmpty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return members_.empty();
}
