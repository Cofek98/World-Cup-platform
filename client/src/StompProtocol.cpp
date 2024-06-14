#include "../include/StompProtocol.h"


class ConnectionHandler;
class User;

StompProtocol::StompProtocol(ConnectionHandler& handler):
    hand(&handler), 
    user(nullptr), 
    receiptId(1), 
    actions(unordered_map<int, vector<int>>()) {}

StompProtocol::StompProtocol(StompProtocol& other): 
    hand(other.hand), 
    user(other.user), 
    receiptId(other.receiptId), 
    actions(other.actions){}

StompProtocol::~StompProtocol(){
    if (hand)
        delete(hand);
    if (user)
        delete(user);
}

StompProtocol& StompProtocol::operator=(StompProtocol& other){
    if (&other != this){
        hand = other.hand;
        user = other.user;
        receiptId= other.receiptId;
        actions = other.actions;
    }
    return *this;
}


string StompProtocol::process(string msg){
    vector<string> msgVec;
    while (msg != "") {
        string msgPart;
        if (msg.find(" ") == std::string::npos) {
            msgVec.push_back(msg);
            break;
        }
        msgPart = msg.substr(0, msg.find(" "));
        msgVec.push_back(msgPart);
        msg = msg.substr(msgPart.length() + 1, msg.length());
    }

    string frame = "";
    if (msgVec[0] == "login"){
        frame = login(msgVec);
        return frame;
    }

    if (user == nullptr){
        std::cout << "Client is not connected\n" << std::endl;
    }

    else {
        if (msgVec[0] == "join"){
            frame = join(msgVec);
            return frame;
        }
        if (msgVec[0] == "exit") {
            frame = exit(msgVec);
            return frame;
        }
        if (msgVec[0] == "report"){
            frame = report(msgVec);
            return frame;
        }
        if (msgVec[0] == "summary"){
            summary(msgVec);
            return frame;
        }
        if (msgVec[0] == "logout"){
            frame = logout(msgVec);
            return frame;
        }

    }

    return frame;
}

string StompProtocol::login(vector<string> msg){
    if (user != nullptr){
        std::cout << "The client is already logged in, log out before trying again\n" << std::endl;
        return "";
    }
    if(msg.size() != 4){
        std::cout << "malformd command(login)\n" << std::endl;
        return "";
    }
    
    if (msg[1].find(":") == std::string::npos){
        std::cout << "malformd command(login-host:port)\n" << std::endl;
        return "";
    }
    int ind = msg[1].find(":");
    string host = msg[1].substr(0,ind);
    string port = msg[1].substr(ind+1,msg[1].length());
    string userName = msg[2];
    string pass = msg[3];
    hand->setHP(host, port);
    if (!hand->connect()){
        std::cout << "Could not connect to server\n" << std::endl;
        return "";
    }
    this->user = new User(userName, pass);
    string frame;
    frame = frame + "CONNECT" + "\n" + "accept-version:1.2" + "\n" + "host:" + host + "\n"
    + "login:" + userName + "\n" + "passcode:" + pass + "\n";

    return frame;
}

string StompProtocol::join(vector<string> msg){
    if(msg.size() != 2){
        std::cout << "malformd command(join)\n" << std::endl;
        return "";
    }

    string game = msg[1];
    if (game.find("_") == std::string::npos){
        std::cout << "malformd command(join-game name is illegal)\n" << std::endl;
        return "";
    }

    int subId = user->getSub();
    string sub = to_string(subId);
    string receipt = to_string(receiptId);
    vector<int> action;
    action.push_back(1);
    action.push_back(subId);
    actions.insert({receiptId, action});
    receiptId++;
    user->waitForSubscribed(subId, game);
    string frame;
    frame = frame + "SUBSCRIBE" + "\n" + "destination:/" + game + "\n" + "id:" + sub + "\n" +
    "receipt:" + receipt + "\n";
    return frame;
}


string StompProtocol::exit(vector<string> msg){
    if(msg.size() != 2){
        std::cout << "malformd command(exit)\n" << std::endl;
        return "";
    }

    string game = msg[1];
    if (game.find("_") == std::string::npos){
        std::cout << "malformd command(exit-game name is illegal)\n" << std::endl;
        return "";
    }

    if (user->subsByTopic.count(game) == 0){
        std::cout << "user is not subscribed to this topic\n" << std::endl;
        return "";    
    }

    int subId = user->subsByTopic.at(game);
    string sub = to_string(subId);
    string receipt = to_string(receiptId);
    vector<int> action;
    action.push_back(2);
    action.push_back(subId);
    actions.insert({receiptId, action});
    receiptId++;
    string frame;
    frame = frame + "UNSUBSCRIBE" + "\n" + "id:" + sub + "\n" + "receipt:" + receipt + "\n";
    return frame;
}

string StompProtocol::logout(vector<string> msg){
    if(msg.size() != 1){
        std::cout << "malformd command(logout)\n" << std::endl;
        return "";
    }

    string receipt = to_string(receiptId);
    vector<int> action;
    action.push_back(0);
    actions.insert({receiptId, action});
    receiptId++;
    string frame;
    frame = frame + "DISCONNECT" + "\n"  + "receipt:" + receipt + "\n";
    return frame;
}


void StompProtocol::processFrame(string frame){
    vector<string> msgVec;
    while (frame != "") {
        string msgPart;
        if (frame.find("\n") == std::string::npos) {
            msgVec.push_back(frame);
            break;
        }
        msgPart = frame.substr(0, frame.find("\n"));
        msgVec.push_back(msgPart);
        frame = frame.substr(msgPart.length() + 1, frame.length());
    }

    if (!msgVec.empty()){

        if (msgVec[0] == "CONNECTED"){
            connected();
        }
        if (msgVec[0] == "MESSAGE"){
            message(msgVec);
        }
        if (msgVec[0] == "RECEIPT"){
            receipt(msgVec);
        }
        if (msgVec[0] == "ERROR"){
            error(msgVec);
        }
    }
}

void StompProtocol::connected(){
    user->connect();
    std::cout << "Login successful\n" << std::endl;
}

void StompProtocol::receipt(vector<string> msgVec){
    string receipt = msgVec[1].substr(msgVec[1].find(":") + 1 , msgVec[1].length());
    int recId = std::stoi(receipt);
    vector<int> action = actions.at(recId);
    actions.erase(recId);
    if (action[0] == 0){
        disconnect();
    }
    else{
        int subId = action[1];
        if (action[0] == 1){    
            user->subscribed(subId);
        }
        else {
            user->unsubscribed(subId);
        }
    }
}
void StompProtocol::disconnect(){
    user->disconnect();
    user = nullptr;
    hand->close();
}

void StompProtocol::error(vector<string> msgVec){
    string error = msgVec[1].substr(msgVec[1].find(":") + 1 , msgVec[1].length());
    std::cout << "ERROR\n" + error + "\n" + "disconnecting" << std::endl;
    disconnect();
}

string StompProtocol::report(vector<string> msg){
    if(msg.size() != 2){
        std::cout << "malformd command(report)\n" << std::endl;
        return "";
    }

    string file = "data/" + msg[1];
    names_and_events nae = ::parseEventsFile(file);
    string a = nae.team_a_name;
    string b = nae.team_b_name;
    vector<Event> events = nae.events;
    string frame = "";
    frame = frame + "SEND" + "\n" + "destination:/" + a + "_" + b + "\n" + "\n" + "user: " + 
        user->getName() + "\n" + "team a: " + a + "\n" + "team b: " + b + "\n";
    for (Event e : events){
        string currFrame = frame;
        currFrame = currFrame + "event name: " + e.get_name() + "\n" + 
        "time: " + to_string(e.get_time()) + "\n" +
        "general game updates:\n";
        map<string, string> up = e.get_game_updates();
        for (auto iter = up.begin(); iter != up.end(); iter++){
            currFrame = currFrame + iter->first + ": " + iter->second + "\n";
        }
        currFrame = currFrame + "team a updates:" + "\n";
        map<string, string> upa = e.get_team_a_updates();
        for (auto iter = upa.begin(); iter != upa.end(); iter++){
            currFrame = currFrame + iter->first + ": " + iter->second + "\n";
        }
        currFrame = currFrame + "team b updates:" + "\n";
        map<string, string> upb = e.get_team_b_updates();
        for (auto iter = upb.begin(); iter != upb.end(); iter++){
            currFrame = currFrame + iter->first + ": " + iter->second + "\n";
        }
        currFrame = currFrame + "description:" + "\n" + e.get_discription() + "\n";
        hand->sendFrameAscii(currFrame, '\0');
    }
    
    return "";
}


void StompProtocol::summary(vector<string> vec){
    string game = vec[1];
    string userName = vec[2];
    string file = vec[3];
    string out = "";
    if (user->games.count(game) == 0 || user->games.at(game).count(userName) == 0){
        std::cout << "no reports from " + userName + " for " + game +"\n" << std::endl;
    } else{
        GameSum sum = user->games.at(game).at(userName);
        out = out + sum.a_name + " vs " + sum.b_name + "\n" + "Game stats:" + "\n" + "General stats:" + "\n";
        for (auto iter = sum.game_stats.begin(); iter != sum.game_stats.end(); iter++){
            out = out + iter->first + ":" + iter->second + "\n";
        }
        out = out + sum.a_name + " stats:" + "\n";
        for (auto iter = sum.team_a_stats.begin(); iter != sum.team_a_stats.end(); iter++){
            out = out + iter->first + ":" + iter->second + "\n";
        }
        out = out + sum.b_name + " stats:" + "\n";
        for (auto iter = sum.team_b_stats.begin(); iter != sum.team_b_stats.end(); iter++){
            out = out + iter->first + ":" + iter->second + "\n";
        }
        out = out + "Game event reports:" + "\n";
        for (auto iter = sum.descriptions.begin(); iter != sum.descriptions.end(); iter++){
            out = out + iter->second + "\n\n\n";
        }

        ofstream stream;
        stream.open(file, std::ios::out);
        if (stream.is_open()){
            stream << out << endl;
            stream.close();
        }
    }
    
}



void StompProtocol::message(vector<string> msgVec){

    string dest = msgVec[3].substr(msgVec[3].find(":") + 2 , msgVec[3].length());
    int ind = dest.find("_");
    string a_name = dest.substr(0, ind);
    string b_name = dest.substr(ind+1, dest.length());
    string gameName = dest;
    string sender = msgVec[5].substr(msgVec[5].find(":") + 2 , msgVec[5].length());
    if (user->games.count(gameName) == 0){
        unordered_map<string, GameSum> ugs = unordered_map<string, GameSum>();
        GameSum sum = GameSum();
        sum.start(gameName, a_name, b_name);
        ugs.insert({sender, sum});
        user->games.insert({gameName, ugs});
    } else if (user->games.at(gameName).count(sender) == 0){
        GameSum sum = GameSum();
        sum.start(gameName, a_name, b_name);
        user->games.at(gameName).insert({sender,sum});   
    }
    user->games.at(gameName).at(sender).process(msgVec);
    
}





