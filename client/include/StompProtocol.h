#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/User.h"
#include <unordered_map>
#include "../include/event.h"
#include <iostream>
#include <fstream>


class User;
class ConnectionHandler;

class StompProtocol
{

private:
    ConnectionHandler* hand;
    User* user;
    int receiptId;
    unordered_map<int, vector<int>> actions;

public:
    StompProtocol(ConnectionHandler& hand);
    StompProtocol(StompProtocol& other);
    ~StompProtocol();
    StompProtocol& operator=(StompProtocol& other);
    string process(string msg);
    string login(vector<string>);
    string join(vector<string>);
    string exit(vector<string>);
    string report(vector<string>);
    void summary(vector<string>);
    string logout(vector<string>);
    void processFrame(string frame); 
    void connected();
    void message(vector<string>);
    void receipt(vector<string>);
    void error(vector<string>);
    void disconnect();
};
