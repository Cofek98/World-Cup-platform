#pragma once

#include <string>
#include <vector>
#include <map>

using std::vector;
using namespace std;

class GameSum{

    public:

        string name;
        string a_name;
        string b_name;
        map<string, string> game_stats;
        map<string, string> team_a_stats;
        map<string, string> team_b_stats;
        map<string, string> descriptions;
        bool beforeHT;

        GameSum();
        void start(string, string, string);
        void process(vector<string>);
        vector<string> split(string);
        string summary();

};