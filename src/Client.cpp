#include "Client.hpp"
#include <utility>

Client::Client(Socket socket)
    : socket_(std::move(socket)),
      username_("Guest" + std::to_string(socket_.fd())) {}