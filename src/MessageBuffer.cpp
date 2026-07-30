#include "MessageBuffer.hpp"

void MessageBuffer::append(std::string_view data)
{
    buffer_ += data;
}

std::optional<std::string> MessageBuffer::nextMessage()
{
    size_t pos = buffer_.find('\n');

    if (pos == std::string::npos) {
        return std::nullopt;
    }

    std::string message = buffer_.substr(0, pos);

    if (!message.empty() && message.back() == '\r')
        message.pop_back();

    buffer_.erase(0, pos + 1);

    return message;
}