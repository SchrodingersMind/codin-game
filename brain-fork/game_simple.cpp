//
// Created by robotknik on 05.02.22.
//

#include "game_simple.h"

char abc[] = {' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };

int getNumber(char ch) {
    if (ch == ' ')
        return 0;
    return ch - 'A' + 1;
}

int getDifference(char start, char end) {
    int start_p = getNumber(start), end_p = getNumber(end);
    int diff = max(start_p, end_p) - min(start_p, end_p);
    if (diff > 13) {
        diff = 27-diff;
        if (start_p < end_p) {
            diff = -diff;
        }
    } else if (start_p > end_p) {
        diff = -diff;
    }
    return diff;
}


void GameSimple::moveLeft(int repeat) {
    blub_pos -= repeat;
    if (blub_pos < 0) {
        blub_pos = 30 + blub_pos;
    }
}
void GameSimple::moveRight(int repeat) {
    blub_pos += repeat;
    if (blub_pos > 30) {
        blub_pos = blub_pos - 30;
    }
}
vector<int> GameSimple::getNeighbors() {
    vector<int> result;
    for (int i=blub_pos-14; i<blub_pos+15;i++) {
        result.push_back(i%30);
    }
    return result;
}

string GameSimple::findBestOneWay(char ch) {
    string result;
    vector<int> neighbors = getNeighbors();
    int min_value = 100;
    int min_index = 0;
    for (int i=0; i<neighbors.size(); i++) {
        int tmp_diff = abs(getDifference(runes[neighbors[i]], ch)) + abs((int)neighbors.size()/2 - i);
        if (tmp_diff < min_value) {
            min_value = tmp_diff;
            min_index = i;
        }
    }
//    cerr << "Found nearest move to the " << neighbors[min_index] << " and all moves " << min_value << endl;
    // What to do: move character to the neighbor, rotate rune and spell letter
    if (min_index < (int)neighbors.size()/2) {
        result.resize(result.size()+neighbors.size()/2-min_index, '<');
        moveLeft((int)neighbors.size()/2-min_index);
    } else {
        result.resize(result.size()+min_index-neighbors.size()/2, '>');
        moveRight((int)min_index-neighbors.size()/2);
    }
    int rune_index = neighbors[min_index];
    int diff = getDifference(runes[rune_index], ch);
    if (diff > 0) {
        result.resize(result.size()+diff, '+');
    } else {
        result.resize(result.size()-diff, '-');
    }
    runes[rune_index] = ch;
    result.append(".");
    return result;
}

string GameSimple::findWays(const string& inp_str) {
    string result;
    for (char ch : inp_str) {
        result += findBestOneWay(ch);
    }
    return result;
}