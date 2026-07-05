#pragma once
#include "Socket.hpp"
#include <vector> 
#include <poll.h>

class Server{ 
    public: 
        bool run(u_int16_t);
    private:
        Socket listener_;
        std::vector<pollfd> fds; 
        std::vector<Socket> clients; 

        void acceptClients(); 
        void handleClientData(size_t fd_index);

};