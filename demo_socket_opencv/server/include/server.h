#pragma once

#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <map>
#include "define.h"

class Server{
    private:
        virtual void handle_connection(int sockfd);
    public:
        Server() {}
        void initialize(int port);
        void start_listening(int socket);
        void authenticate(int sockfd);
        void sendMessage(int sockfd, std::string prompt);
        ~Server() {}
};