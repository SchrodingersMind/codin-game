//
// Created by robotknik on 05.02.22.
//

#ifndef BRAINFORK_GAME_SIMPLE_H
#define BRAINFORK_GAME_SIMPLE_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class GameSimple {
private:
    string findBestOneWay(char ch);
    vector<int> getNeighbors();
    void moveLeft(int repeat);
    void moveRight(int repeat);
public:
    vector<char> runes = vector<char>(30, ' ');
    int blub_pos = 15;

    string findWays(const string& inp_str);
};

#endif //BRAINFORK_GAME_SIMPLE_H
