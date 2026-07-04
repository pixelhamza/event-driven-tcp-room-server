#include "Socket.hpp"
#include <iostream>
#include<poll.h>
#include<vector>
#include<utility>

int main() {
    Socket listener;
    if (!listener.listenOn(8080)) {
        std::cerr << "Failed to start server\n";
        return 1;
    }
    std::cout<<"We listening on 8080"<<std::endl;
    
    std::vector<pollfd> fds;
    pollfd listenerPfd;
    listenerPfd.fd = listener.fd();
    listenerPfd.events = POLLIN;
    listenerPfd.revents = 0;

    fds.push_back(listenerPfd);

    std::vector<Socket> clients;

    while( true ){ 
        int ready = poll(fds.data(),fds.size(),-1);
        if(ready < 0){
             std::cerr << "poll failed\n";
            break;

        }

        if (fds[0].revents & POLLIN) {
            Socket client = listener.accept();
            
            if(client.isValid()){
                std::cout<<"New Connection established"<<std::endl;
                pollfd clientPfd;
                clientPfd.fd = client.fd();
                clientPfd.events = POLLIN;
                clientPfd.revents = 0;
                fds.push_back(clientPfd);
                clients.push_back(std::move(client));
            }
            
        }
        for (size_t i = 1; i < fds.size(); ++i) {
            if (fds[i].revents & POLLIN) {
            std::cout << "Client at fds[" << i << "] has data ready\n";
            }
        }

        
    }
    return 0;
    
}