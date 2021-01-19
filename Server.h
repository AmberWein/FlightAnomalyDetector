// Amber Weiner 208783522

/*
 * Server.h
 *
 *  Created on: Dec 13, 2020
 *      Author: Eli
 */

#ifndef SERVER_H_
#define SERVER_H_


#include "commands.h"
#include "CLI.h"

#include <sys/socket.h>	// includes a number of definitions of structures needed for sockets.
#include <netinet/in.h>	// contains constants and structures needed for internet domain adresses.

#include <pthread.h>
#include <thread>

//#include <iostream>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


using namespace std;

// edit your ClientHandler interface here:
class ClientHandler{
    public:
    virtual void handle(int clientID)=0;
};


// you can add helper classes here and implement on the cpp file


// edit your AnomalyDetectionHandler class here
class AnomalyDetectionHandler: public ClientHandler{
	public:
    void handle(int clientID) override{
        SocketIO socketIo(clientID);
        CLI cli(&socketIo);
        cli.start();
    }

//    struct sockaddr_in
//{
//    uint8_t sin_len;         //length of structure 1
//    sa_family_t sin_family;  //AF_INIT
//    in_port_t sin_port;      //16 bit port number
//    struct in_addr sin_addr; //32 bit IPV4 address
//    char sin_zero[8];
//};
};


// implement on Server.cpp
class Server {
	thread* t; // the thread to run the start() method in
    int fd;
    sockaddr_in server;
    sockaddr_in client;
    bool stopAccepting;
	// you may add data members

public:
	explicit Server(int port) throw (const char*);
	virtual ~Server();
	void start(ClientHandler& ch)throw(const char*);
	void stop();
};

#endif /* SERVER_H_ */

