#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

enum Directions {
    kNone,
    kRight,
    kDown,
    kLeft,
    kUp
};


//////// Ball class

class Ball {
public:
    int x, y;
    int shot_count;
    int last_shot_count;
    vector<Directions> directions;

    Ball(int x, int y, int shot_count): x(x), y(y), shot_count(shot_count), last_shot_count(shot_count) {};

    void undo();
    bool is_movable() const;
    void move(Directions dir);
};

void Ball::undo() {
    if (shot_count == last_shot_count) {
        cout << "There is going something fishy -_-" << endl;
    }
    last_shot_count++;
    directions.pop_back();
}
void Ball::move(Directions dir) {
    last_shot_count--;
    directions.push_back(dir);
}
bool Ball::is_movable() const {
    return last_shot_count != 0;
}

bool compareBalls (Ball ball1, Ball ball2) {
    return ball1.shot_count < ball2.shot_count;
}
//////// End of the Ball class



//////// Map class
enum MapElems {
    kLand,
    kBall,
    kPool,
    kHole
};
enum OutMapElems {
    kDot,
    kArrow_right,
    kArrow_down,
    kArrow_left,
    kArrow_up,
    kHole_empty,
    kHole_filled
};

class Map {
private:
    void makeMove();
    void sortBalls();
public:
    int width;
    int height;
    vector<vector<int>> actual_map;
    vector<vector<int>> result_map;
    vector<Ball> balls;

    Map(int width, int height): width(width), height(height) {};

    void input();
    void playGame();
};

void Map::makeMove() {

}

void Map::sortBalls() {

}

void Map::input() {
    for (int l = 0; l < height; l++) {
        string row;
        cin >> row; cin.ignore();
        actual_map.emplace_back(0);
        result_map.emplace_back(0);
        for (int k=0; k < row.size(); k++) {
            char c = row[k];
            switch (c) {
                case 'X':
                    actual_map[l].push_back(MapElems::kPool);
                    result_map[l].push_back(OutMapElems::kDot);
                    break;
                case '.':
                    actual_map[l].push_back(MapElems::kLand);
                    result_map[l].push_back(OutMapElems::kDot);
                    break;
                case 'H':
                    actual_map[l].push_back(MapElems::kHole);
                    result_map[l].push_back(OutMapElems::kHole_empty);
                default:
                    // Probably it's number from 1 to 9, that assume bat shot_count
                    Ball ball = Ball(l, k, c-'0');
                    balls.push_back(ball);
                    actual_map[l].push_back(MapElems::kBall);
                    result_map[l].push_back(OutMapElems::kDot);
                    break;

            }
        }
    }
    // Now balls with lower shot count should go first
    sort(balls.begin(), balls.end(), compareBalls);
}
//////// End of the Map class

int main()
{
    int width;
    int height;
    cin >> width >> height; cin.ignore();
    for (int i = 0; i < height; i++) {
        string row;
        cin >> row; cin.ignore();
    }
}