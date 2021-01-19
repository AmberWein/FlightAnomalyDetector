// Amber Weiner 208783522

#include "Server.h"
#include <chrono>

Server::Server(int port) throw(const char *) {
    this->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        throw "socket failure";
    this->server.sin_family = AF_INET;
    this->server.sin_addr.s_addr = INADDR_ANY;
    this->server.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *) &server, sizeof(server)) < 0)
        throw "bind failure";
    if (listen(fd, 2) < 0) // check how many connections are alowed
        throw "listen failure";
    this->stopAccepting = false;
}

//void threadFunc(ClientHandler &ch) throw(const char *) {
//    socklen_t clientSize = sizeof(client);
//    while (!this->stopAccepting) {
//        int clientAccepted = accept(fd, (struct sockaddr *) &client, &clientSize);
//        if (clientAccepted < 0)
//            throw "accept failure";
//        ch.handle(clientAccepted);
//        close(clientAccepted);
//        //close(fd);
//    }
//    close(fd);
//}

void Server::start(ClientHandler &ch) throw(const char *) {
    t = new thread([&ch, this]() {
        while (!stopAccepting) {
            socklen_t clientSize = sizeof(client);
            int clientAccepted = accept(fd, (struct sockaddr *) &client, &clientSize);
            if (clientAccepted < 0)
                throw "accept failure";
            ch.handle(clientAccepted);
            close(clientAccepted);
            //close(fd);
        }
        close(fd);
    });
}

void Server::stop() {
    this->stopAccepting = true;
    t->join(); // do not delete this!
}

Server::~Server() {
}