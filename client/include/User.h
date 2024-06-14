#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "../include/GameSum.h"

using std::vector;
using namespace std;
//using std::unordered_map;


class User {

private:
    string name;
    string password;
    bool connected;
	unordered_map <int, string> awaitingSubs;
	int subCount;
    

public:
    
    unordered_map <string, int> subsByTopic;
    unordered_map <int , string> subsById;
    unordered_map<string, unordered_map<string, GameSum>> games;
    
    User(string name, string password);
    ~User();
    bool isConnected();
    void connect();
    void disconnect();
    int getSub();
    void waitForSubscribed(int subId, string topic);
    void subscribed(int subId);
    void unsubscribed(int subId);
    void clear();
    string getName();

};