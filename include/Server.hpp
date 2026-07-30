#pragma once
#include "Socket.hpp"
#include "Client.hpp"
#include <vector> 
#include <poll.h>

class Server{ 
    public: 
        bool run(u_int16_t);
    private:
        Socket listener_;
        std::vector<pollfd> fds_; 
        std::vector<Client> clients_; 

        void removeClient(size_t fd_index);

        void acceptNewClient(); 
        void handleClientData(size_t fd_index);

};