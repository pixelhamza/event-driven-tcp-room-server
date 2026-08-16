#pragma once
#include <string>
#include <string_view>

enum class CommandType{
    NICK,
    JOIN,
    LEAVE,
    ROOMS,
    USERS,
    HELP,
    MSG,
    CHAT_MESSAGE,
    UNKNOWN
};

struct Command{ 
    CommandType type;
    std::string arg;
    std::string rawCommand;
};

class Protocol{
public:
    static Command parse(std::string_view input);
};