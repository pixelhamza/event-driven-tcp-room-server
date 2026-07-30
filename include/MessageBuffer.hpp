#pragma once 
#include <string>
#include <string_view>
#include <optional>

class MessageBuffer {
public:
    void append(std::string_view data);
    std::optional<std::string> nextMessage();

private:
    std::string buffer_;
};

