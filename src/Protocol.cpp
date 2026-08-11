#include "Protocol.hpp"

Command Protocol::parse(std::string_view input) {
    if (input.empty()) {
        return Command{CommandType::UNKNOWN, "", ""};
    }

    std::string raw(input);

    if (input.front() != '/') {
        return Command{CommandType::CHAT_MESSAGE, raw, raw};
    }

    // so we split split the command name and arguments by space
    size_t spacePos = input.find(' ');
    std::string_view cmdStr;
    std::string_view argStr;

    if (spacePos == std::string_view::npos) {
        cmdStr = input;
    } else {
        cmdStr = input.substr(0, spacePos);
        // Find start of argument (skipping extra spaces)
        size_t argStart = input.find_first_not_of(' ', spacePos);
        if (argStart != std::string_view::npos) {
            argStr = input.substr(argStart);
        }
    }

    CommandType type = CommandType::UNKNOWN;

    if (cmdStr == "/nick") {
        type = CommandType::NICK;
    } else if (cmdStr == "/join") {
        type = CommandType::JOIN;
    } else if (cmdStr == "/leave") {
        type = CommandType::LEAVE;
    } else if (cmdStr == "/rooms") {
        type = CommandType::ROOMS;
    } else if (cmdStr == "/users") {
        type = CommandType::USERS;
    } else if (cmdStr == "/help") {
        type = CommandType::HELP;
    }

    return Command{type, std::string(argStr), raw};
}
