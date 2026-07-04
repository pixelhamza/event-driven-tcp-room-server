#include "Socket.hpp"
#include <sys/socket.h>
#include <unistd.h>

Socket::Socket() { fd_ = socket(AF_INET, SOCK_STREAM, 0); }

Socket::~Socket() {
  if (fd_>= 0)
    close(fd_);
}
