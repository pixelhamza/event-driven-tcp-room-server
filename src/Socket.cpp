#include "Socket.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include<iostream>

Socket::Socket() { fd_ = socket(AF_INET, SOCK_STREAM, 0); }

Socket::~Socket() {
  if (fd_>= 0)
    close(fd_);
}

Socket::Socket(int fd): fd_(fd) {};

bool Socket::listenOn(uint16_t port){ 
  int opt = 1;
  sockaddr_in addr{};
  addr.sin_family = AF_INET; 
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  if(bind(fd_,reinterpret_cast<sockaddr*>(&addr),sizeof(addr)) < 0){
    std::cerr<< " bind failed " << strerror(errno) << "\n";
    return false;
  }
  if(listen(fd_,16) < 0){ 
    std::cerr << "listen failed: " << strerror(errno) << "\n";
    return false;
  }
  return true;
}

Socket Socket::accept(){ 
  sockaddr_in clientAddr{};
  socklen_t addrLen = sizeof(clientAddr);

  int clientFd = ::accept(fd_,reinterpret_cast<sockaddr*>(&clientAddr),&addrLen);
  if(clientFd < 0){
    std::cerr << "accept failed: " <<strerror(errno)<< "\n";
    return Socket(-1);
  }

  return Socket(clientFd);
}

Socket::Socket(Socket&& other) noexcept : fd_(other.fd_) {
    other.fd_ = -1;
}

Socket& Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {         
        if (fd_ >= 0) {
            close(fd_);             
        }
        fd_ = other.fd_;             
        other.fd_ = -1;              
    }
    return *this;
}

int Socket :: recv(char* buffer,size_t bufferSize){ 
  ssize_t n = ::recv(fd_,buffer,bufferSize,0);

  return static_cast<int>(n);
}

bool Socket::sendAll(const char* data,size_t length){ 
  size_t total_sent = 0;
  while(total_sent < length){ 
    ssize_t sent = ::send(fd_, data + total_sent, length - total_sent,0);
    if (sent <= 0) {
        return false;  
      }
      total_sent += sent;
  }
  return true;
}