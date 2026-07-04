#include "Socket.hpp"
#include <iostream>

int main() {
    Socket listener;
    listener.listenOn(8080);
    if (!listener.listenOn(8080)) {
        std::cerr << "Failed to listen on port 8080\n";
        return 1;
    }
    std::cout<<" we listening on 8080"<<std::endl;
    Socket client = listener.accept();
    if(client.isValid()){
        std::cout<<"we locked in boi";
    }
}