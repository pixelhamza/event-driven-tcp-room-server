#include "RoomManager.hpp"
#include <sstream>
#include <iostream>

RoomManager::RoomManager() {
    // Ensure default "#lobby" room exists on server start
    getOrCreateRoom("#lobby");
}

Room* RoomManager::getOrCreateRoom(const std::string& roomName) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string name = roomName;
    if (name.empty()) {
        name = "#lobby";
    } else if (name.front() != '#') {
        name = "#" + name;
    }

    auto it = rooms_.find(name);
    if (it == rooms_.end()) {
        auto room = std::make_unique<Room>(name);
        Room* ptr = room.get();
        rooms_[name] = std::move(room);
        return ptr;
    }
    return it->second.get();
}

void RoomManager::moveClient(Client* client, const std::string& targetRoomName) {
    if (!client) return;

    Room* oldRoom = client->currentRoom();
    Room* newRoom = getOrCreateRoom(targetRoomName);

    if (oldRoom == newRoom) return;

    if (oldRoom) {
        oldRoom->removeClient(client);
    }

    client->setCurrentRoom(newRoom);
    newRoom->addClient(client);

    // Delete empty rooms (except default #lobby)
    if (oldRoom && oldRoom->isEmpty() && oldRoom->name() != "#lobby") {
        std::lock_guard<std::mutex> lock(mutex_);
        rooms_.erase(oldRoom->name());
    }
}

void RoomManager::removeClient(Client* client) {
    if (!client) return;

    Room* current = client->currentRoom();
    if (current) {
        current->removeClient(client);
        client->setCurrentRoom(nullptr);

        if (current->isEmpty() && current->name() != "#lobby") {
            std::lock_guard<std::mutex> lock(mutex_);
            rooms_.erase(current->name());
        }
    }
}

std::string RoomManager::listRooms() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;
    oss << "=== Active Rooms ===\n";
    for (const auto& [name, room] : rooms_) {
        oss << " * " << name << " (" << room->memberCount() << " users)\n";
    }
    return oss.str();
}
