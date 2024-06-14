#include "../include/GameSum.h"

GameSum::GameSum():
    name(""),
    a_name(""),
    b_name(""),
    game_stats(map<string, string>()),
    team_a_stats(map<string, string>()),
    team_b_stats(map<string, string>()),
    descriptions(map<string, string>()),
    beforeHT(true)

{}

void GameSum::start(string gameName, string a, string b){
    name = gameName;
    a_name = a;
    b_name = b;
    
}

void GameSum::process(vector<string> vec){
    string eventName = vec[8].substr(vec[8].find(":") + 1, vec[8].length());
    string time = vec[9].substr(vec[9].find(":") + 2, vec[9].length());
    int i = 11;
    while(vec[i] != "team a updates:"){
        string stat = vec[i].substr(0, vec[i].find(":"));
        string value = vec[i].substr(vec[i].find(":") + 1, vec[i].length());
        if (stat == "before halftime"){
            if (value.find("false") != string::npos){
                beforeHT = false;
            }
        }
        if (game_stats.count(stat) == 0){
            game_stats.insert({stat, value});
        }
        else{
            game_stats.at(stat) = value;
        }
        i++;
    }
    i++;
    while(vec[i] != "team b updates:"){
        string stat = vec[i].substr(0, vec[i].find(":"));
        string value = vec[i].substr(vec[i].find(":") + 1, vec[i].length());
        if (team_a_stats.count(stat) == 0){
            team_a_stats.insert({stat, value});
        }
        else{
            team_a_stats.at(stat) = value;
        }
        i++;
    }
    i++;
    while(vec[i] != "description:"){
        string stat = vec[i].substr(0, vec[i].find(":"));
        string value = vec[i].substr(vec[i].find(":") + 1, vec[i].length());
        if (team_b_stats.count(stat) == 0){
            team_b_stats.insert({stat, value});
        }
        else{
            team_b_stats.at(stat) = value;
        }
        i++;
    }
    i++;

    string desc = time + " -" + eventName + ":" + "\n\n" + vec[i];
    if (beforeHT){
        time = "a" + time;
    } else {
        time = "b" + time;
    }
    descriptions.insert({time, desc});

}

vector<string> GameSum::split(string frameBody){
    int ind;
    vector<string> vec;
    while(frameBody != ""){
        ind = frameBody.find("\n");
        vec.push_back(frameBody.substr(0, ind));
        frameBody = frameBody.substr(ind+1, frameBody.length());
    }
    return vec;
}




