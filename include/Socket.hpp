#include <cstdint> 
class Socket {
private:
  int fd_;

public:
  Socket();
  ~Socket();
  explicit Socket(int fd);
  bool listenOn(uint16_t port);
  int fd() const { return fd_;}
  int isValid() const { return fd_>=0;}
  Socket accept();
};
