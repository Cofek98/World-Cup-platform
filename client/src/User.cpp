#include <User.h>
#include <unordered_map>

User::User(string myName, string myPassword) : 
    name(myName), 
    password(myPassword),
    connected(false), 
    awaitingSubs(unordered_map<int,string>()),
    subCount(1), 
    subsByTopic(unordered_map<string, int>()), 
    subsById(unordered_map<int, string>()),
    games(unordered_map<string, unordered_map<string, GameSum>>())
    {}

User::~User() {
    disconnect();
}

bool User::isConnected(){
    return connected;
}

void User::connect(){
    connected = true;
}

void User::disconnect(){
    connected = false;
    clear();
}

int User::getSub(){
    int sub = subCount;
    subCount++;
    return sub;
}

void User::waitForSubscribed(int subId, string topic){
    awaitingSubs.insert({subId, topic});
}

void User::subscribed(int subId){
    string topic = awaitingSubs.at(subId);
    awaitingSubs.erase(subId);
    subsByTopic.insert({topic, subId});
    subsById.insert({subId, topic});
}

void User::unsubscribed(int subId){
    string topic = subsById.at(subId);
    subsByTopic.erase(topic);
    subsById.erase(subId);
}

void User::clear(){
    awaitingSubs.clear();
    subsByTopic.clear();
    subsById.clear();
}

string User::getName(){
    return name;
}

//unordered_map<string, unordered_map<string, GameSum>>& User::getGames(){
  //  return games;
//}







