#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include "Room.hpp"
#include "Client.hpp"

class RoomManager {
public:
    RoomManager();

    // Gets an existing room or creates a new one if it doesn't exist
    Room* getOrCreateRoom(const std::string& roomName);

    // Moves a client from their current room to the target room
    void moveClient(Client* client, const std::string& targetRoomName);

    // Removes a client from their current room (e.g., on disconnect)
    void removeClient(Client* client);

    // Returns a formatted string list of all active rooms and member counts
    std::string listRooms() const;

private:
    std::unordered_map<std::string, std::unique_ptr<Room>> rooms_;
    mutable std::mutex mutex_;
};
