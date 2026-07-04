#include <cstdint> 
#include<cstddef>
class Socket {
private:
  int fd_;

public:
  Socket();
  ~Socket();
  explicit Socket(int fd);
  bool listenOn(uint16_t port);

  Socket(const Socket&) = delete;
  Socket& operator=(const Socket&) = delete;//ownership transfer

  Socket(Socket&& other) noexcept;
  Socket& operator=(Socket&& other) noexcept;

  int fd() const { return fd_;}
  int isValid() const { return fd_>=0;}
  Socket accept();
  bool sendAll(const char* data , size_t length);
  int recv(char* buffer, size_t bufferSize);
};
