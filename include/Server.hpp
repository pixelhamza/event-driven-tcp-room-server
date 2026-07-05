#pragma once
#include "Socket.hpp"
#include <vector> 
#include <poll.h>

class Server{ 
    public: 
        bool run(u_int16_t);
    private:
        Socket listener_;
        std::vector<pollfd> fds_; 
        std::vector<Socket> clients_; 

        void acceptNewClient(); 
        void handleClientData(size_t fd_index);

};