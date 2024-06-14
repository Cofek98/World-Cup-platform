#pragma once

#include <ConnectionHandler.h>
#include <StompProtocol.h>

class Kthread{


    private:
        ConnectionHandler* hand;
        StompProtocol* protocol;

    public:
        Kthread(ConnectionHandler& hand, StompProtocol& protocol);
        ~Kthread();
        Kthread(const Kthread& other);
        Kthread & operator=(const Kthread& other);
        void run();

};