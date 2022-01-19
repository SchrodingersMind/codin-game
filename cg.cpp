#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

enum Moves {
    kNone,
    kSpeed,
    kSlow,
    kJump,
    kUp,
    kDown
};


class Position {
public:
    int lin, col;
};

//class Bike {
//public:
//    Position pos;
//    bool is_destroyed = false;
//    Bike(int line, int col): pos(Position {line, col}) {};
//
//    bool isTop () const;
//    bool isDown () const;
//};
//
//bool Bike::isDown() const {
//    return pos.lin == 3;
//}
//bool Bike::isTop() const {
//    return pos.col == 0;
//}


class LineOfBikes {
public:
    vector<int> cur_pos;
    int count=0;
    int pos_x=0;
    int speed=1;

    LineOfBikes(int count): count(count) {};
    LineOfBikes() {};
    void addBike(int pos);
    void deleteBikes(vector<int> bikes);
    bool isMoveDown();
    bool isMoveTop();
    void move(Moves move);
};

void LineOfBikes::addBike(int pos) {
    cur_pos.push_back(pos);
//    count++;
    sort(cur_pos.begin(), cur_pos.end());
}
void LineOfBikes::deleteBikes(vector<int> bikes) {
    for (int bike : bikes) {
        count--;
        auto elem = find(cur_pos.begin(), cur_pos.end(), bike);
        if (elem != cur_pos.end())
            cur_pos.erase(elem);
    }
}

bool LineOfBikes::isMoveDown() {
    if (cur_pos.back() == 3) {
        return false;
    }
    return true;
}
bool LineOfBikes::isMoveTop() {
    if (cur_pos[0] == 0) {
        return false;
    }
    return true;
}

void LineOfBikes::move(Moves move) {
    switch (move) {
        case (Moves::kSpeed):
            speed++;
            break;
        case (Moves::kSlow):
            speed--;
            break;
        case (Moves::kUp):
            for (int& bike : cur_pos) {
                bike--;
            }
            break;
        case (Moves::kDown):
            for (int& bike : cur_pos) {
                bike++;
            }
            break;
    }
    pos_x += speed;
}



enum MapElems {
    kHole,
    kRoad
};

class Game {
private:
    vector<vector<MapElems>> map;
    LineOfBikes cur_line;
    LineOfBikes start_line;
    vector<Moves> moves;
    bool is_init = true;
    int map_length;
    int motorbikes, to_survive;

    bool isHoleInRange(int start_x, int end_x, int pos_y);
public:
    void inputMap();
    void inputTurn();
    void simulateOneChoice(Moves move, LineOfBikes& bikes);
    Moves simulateOneTurn(vector<LineOfBikes> &prev_statuses, vector<Moves> &prev_moves);
    vector<Moves> getBestMoves(LineOfBikes bikes) const;
    string printNextMove();

};

bool Game::isHoleInRange(int start_x, int end_x, int pos_y) {
    int left_border = min(map_length, end_x+1);
    for (;start_x<left_border; start_x++) {
        if (map[pos_y][start_x] == MapElems::kHole)
            return true;
    }
    return false;
}

void Game::inputMap() {
    cin >> motorbikes; cin.ignore();
    cin >> to_survive; cin.ignore();
    start_line = LineOfBikes(motorbikes);
    for (int line=0; line<4; line++) {
        string l0;
        cin >> l0; cin.ignore();
        map_length = l0.length();
        cerr << "Found map length " << map_length << endl;
        map.emplace_back(0);
        for (char c : l0) {
            if (c == '.')
                map[line].push_back(MapElems::kRoad);
            else
                map[line].push_back(MapElems::kHole);
        }
    }
}

void Game::simulateOneChoice(Moves move, LineOfBikes& bikes) {
    vector<int> to_delete;
    switch (move) {
        case (Moves::kSpeed):
            for (int pos_y : bikes.cur_pos) {
                if (isHoleInRange(bikes.pos_x+1, bikes.pos_x+bikes.speed+1, pos_y))
                    to_delete.push_back(pos_y);
            }
            break;
        case (Moves::kSlow):
            for (int pos_y : bikes.cur_pos) {
                if (isHoleInRange(bikes.pos_x+1, bikes.pos_x+bikes.speed-1, pos_y))
                    to_delete.push_back(pos_y);
            }
            break;
        case (Moves::kJump):
            for (int pos_y : bikes.cur_pos) {
                int x_cell = min(map_length-1, bikes.pos_x+bikes.speed);
                if (map[pos_y][x_cell] == MapElems::kHole)
                    to_delete.push_back(pos_y);
            }
            break;
        case (Moves::kDown):
            for (int pos_y : bikes.cur_pos) {
                if (isHoleInRange(bikes.pos_x+1, bikes.pos_x+bikes.speed-1, pos_y) || isHoleInRange(bikes.pos_x+1, bikes.pos_x+bikes.speed, pos_y+1))
                    to_delete.push_back(pos_y);
            }
            break;
        case (Moves::kUp):
            for (int pos_y : bikes.cur_pos) {
                if (isHoleInRange(bikes.pos_x+1, bikes.pos_x+bikes.speed-1, pos_y) || isHoleInRange(bikes.pos_x+1, bikes.pos_x+bikes.speed, pos_y-1))
                    to_delete.push_back(pos_y);
            }
            break;
    }
    bikes.deleteBikes(to_delete);
    bikes.move(move);
}

void Game::inputTurn() {
    int s; // the motorbikes' speed
    cin >> s; cin.ignore();
    if (is_init) {
        start_line.speed = s;
    }
    for (int i = 0; i < motorbikes; i++) {
        int x; // x coordinate of the motorbike
        int y; // y coordinate of the motorbike
        int a; // indicates whether the motorbike is activated "1" or destroyed "0"
        cin >> x >> y >> a; cin.ignore();
        if (is_init) {
            start_line.addBike(y);
        }
    }
    if (is_init) {
        vector<LineOfBikes> bikes {start_line};
        simulateOneTurn(bikes, moves);
    }
    is_init = false;
}

vector<Moves> Game::getBestMoves(LineOfBikes bikes) const {
    vector<Moves> result {Moves::kSpeed};
    if (bikes.isMoveTop())
        result.push_back(Moves::kUp);
    if (bikes.isMoveDown())
        result.push_back(Moves::kDown);
    if (bikes.speed == 1) {
        return result;
    }
    result.push_back(Moves::kJump);
    result.push_back(kSlow);
    return result;
}

Moves Game::simulateOneTurn(vector<LineOfBikes> &prev_statuses, vector<Moves> &prev_moves) {
    LineOfBikes game_status = prev_statuses.back();
    vector<Moves> in_progress = getBestMoves(game_status);
    for (Moves move: in_progress) {
        LineOfBikes tmp_status = game_status;
        simulateOneChoice(move, tmp_status);
        if (tmp_status.count >= to_survive) {
            cerr << "Level: " << prev_moves.size() << " Found temp solution " << move << endl;
            cerr << tmp_status.cur_pos[0] << " " << tmp_status.pos_x << " " << tmp_status.speed << endl;
            prev_statuses.push_back(tmp_status);
            prev_moves.push_back(move);
            if (tmp_status.pos_x > map_length) {
                cerr << "Found final result " << move << endl;
                return move;
            }
            Moves res_move = simulateOneTurn(prev_statuses, prev_moves);
            if (res_move == kNone) {
                prev_statuses.pop_back();
                prev_moves.pop_back();
            } else {
                return res_move;
            }
        }
    }
    return kNone;
}

string Game::printNextMove() {
    Moves next_move = moves[0];
    moves.erase(moves.begin());
    switch (next_move) {
        case (Moves::kDown):
            return "DOWN";
        case (Moves::kUp):
            return "UP";
        case (Moves::kSpeed):
            return "SPEED";
        case (Moves::kSlow):
            return "SLOW";
        case (Moves::kJump):
            return "JUMP";
    }
    return "WAIT";
}

int main()
{
    Game game;
    game.inputMap();
    // game loop
    while (1) {
        game.inputTurn();
        // A single line containing one of 6 keywords: SPEED, SLOW, JUMP, WAIT, UP, DOWN.
        cout << game.printNextMove() << endl;
    }
}