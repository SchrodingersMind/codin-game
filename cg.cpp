#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

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

class Game {
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

void Game::moveLeft(int repeat) {
    blub_pos -= repeat;
    if (blub_pos < 0) {
        blub_pos = 30 + blub_pos;
    }
}
void Game::moveRight(int repeat) {
    blub_pos += repeat;
    if (blub_pos > 30) {
        blub_pos = blub_pos - 30;
    }
}
vector<int> Game::getNeighbors() {
    vector<int> result;
//    if (blub_pos == 0) {
//        return {30-1, 0, 1};
//    } else if (blub_pos == 30-1) {
//        return {30-2, 30-1, 0};
//    }
//    return {blub_pos-1, blub_pos, blub_pos+1};
    for (int i=blub_pos-14; i<blub_pos+15;i++) {
        result.push_back(i%30);
    }
    return result;
}

string Game::findBestOneWay(char ch) {
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
    cerr << "Found nearest move to the " << neighbors[min_index] << " and all moves " << min_value << endl;
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

string Game::findWays(const string& inp_str) {
    string result;
    for (char ch : inp_str) {
        result += findBestOneWay(ch);
    }
    return result;
}

int main()
{
    Game game;
    string magic_phrase;
    getline(cin, magic_phrase);
    cerr << "Secret message is: " << magic_phrase << endl;

    cout << game.findWays(magic_phrase) << endl;
}