#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "point.h"
#include "jps_element.h"

using namespace std;

enum MapElem {
    kNone,
    kWall
};

template <typename T>
T& getElem(vector<vector<T>>& some_map, Point pos) {
    return some_map[pos.r][pos.c];
}

class Game {
private:
    vector<vector<MapElem>> map;
    vector<vector<JPSElem>> jps_map;
    int width, height;

    void setJumpPoints();
    void setStraightJumps(Point start_point, Point direction);
    void setDiagonalJumps(Point start_point, Point direction);
public:

    Game();
    void outResult();
    void proceedMap();
    bool isAvailable(Point cur_point);
};

Game::Game() {
    cin >> width >> height; cin.ignore();
    map.resize(height);
    jps_map.resize(height);
    for (int i = 0; i < height; i++) {
        jps_map[i].resize(width);
        string row; // A single row of the map consisting of passable terrain ('.') and walls ('#')
        cin >> row; cin.ignore();
        for (char& c : row) {
            if (c == '.') {
                map[i].push_back(MapElem::kNone);
            } else {
                map[i].push_back(MapElem::kWall);
            }
        }
    }
}

void Game::setJumpPoints() {
    for (int r=0; r<height; r++) {
        for (int c=0; c<width; c++) {
            if (map[r][c] != MapElem::kWall)
                continue;
            Point cur_p{r, c};
            bool is_north = isAvailable(cur_p+kNorth);
            bool is_south = isAvailable(cur_p+kSouth);
            if (isAvailable(cur_p+kEast)) {
                if (is_north && isAvailable(cur_p+kEast+kNorth))
                    getElem(jps_map, cur_p+kEast+kNorth).addJumpPoint(kEast, kNorth);
                if (is_south && isAvailable(cur_p+kEast+kSouth))
                    getElem(jps_map, cur_p+kEast+kSouth).addJumpPoint(kEast, kSouth);
            }
            if (isAvailable(cur_p+kWest)) {
                if (is_north && isAvailable(cur_p+kWest+kNorth))
                    getElem(jps_map, cur_p+kWest+kNorth).addJumpPoint(kWest, kNorth);
                if (is_south && isAvailable(cur_p+kWest+kSouth))
                    getElem(jps_map, cur_p+kWest+kSouth).addJumpPoint(kWest, kSouth);
            }
        }
    }
}

void Game::setStraightJumps(Point start_point, Point direction) {
    int count = -1;
    bool wasJumpPoint = false;

    for (; start_point.r < height && start_point.c < width && start_point.r >= 0 && start_point.c >= 0; start_point += direction) {
        if (getElem(map, start_point) == MapElem::kWall) {
            count = -1;
            wasJumpPoint = false;
            continue;
        }
        count++;
        if (wasJumpPoint) {
            getElem(jps_map, start_point).setDistance(-direction, count);
        } else {
            getElem(jps_map, start_point).setDistance(-direction, -count);
        }
        if (getElem(jps_map, start_point).isJumpPoint(-direction)) {
            wasJumpPoint = true;
            count = 0;
        }
    }
}

void Game::setDiagonalJumps(Point start_point, Point direction) {
    int count = -1;
    bool wasJumpPoint = false;
    // As we know, direction - diagonal
    // So it could be interpreted as summary of the two cardinal directions
    Point projection_r = Point{direction.r, 0};
    Point projection_c = Point{0, direction.c};

    for (; start_point.r < height && start_point.c < width && start_point.r >= 0 && start_point.c >= 0; start_point += direction) {
        if (getElem(map, start_point) == MapElem::kWall) {
            count = -1;
            wasJumpPoint = false;
            continue;
        } else if (!isAvailable(start_point-direction) || !isAvailable(start_point-projection_r)  || !isAvailable(start_point-projection_c)) {
            count = -1;
            wasJumpPoint = false;
        }
        count += 1;
        JPSElem& cur_element = getElem(jps_map, start_point);
        if (wasJumpPoint) {
            cur_element.setDistance(-direction, count);
        } else {
            cur_element.setDistance(-direction, -count);
        }

        if (cur_element.getDistance(-projection_r) > 0 || cur_element.getDistance(-projection_c) > 0) {
            wasJumpPoint = true;
            count = 0;
        }
    }
}

void Game::proceedMap() {
    // Primary jumps
    cerr << "Set jump points" << endl;
    setJumpPoints();

    // Straight
    cerr << "Set straight jumps" << endl;
    for (int r=0; r<height; r++) {
        setStraightJumps({r, 0}, kEast);
        setStraightJumps({r, width-1}, kWest);
    }
    for (int c=0; c<width; c++) {
        setStraightJumps({0, c}, kSouth);
        setStraightJumps({height-1, c}, kNorth);
    }

    // Now set diagonal jumps...
    cerr << "Set diagonal points" << endl;

    for (int cur=1; cur<width+height-1; cur++) {
        int cur_r=0, cur_c=0;
        if (cur < height) {
            cur_r = cur;
        } else {
            cur_c = cur-height;
        }
        setDiagonalJumps({cur_r, cur_c}, kSouth+kEast);

        cur_r=height-1, cur_c=width-1;
        if (cur < height) {
            cur_r = cur-1;
        } else {
            cur_c = cur-height+1;
        }
        setDiagonalJumps({cur_r, cur_c}, kNorth+kWest);

        cur_r=height-1, cur_c=0;
        if (cur < height) {
            cur_r = cur;
        } else {
            cur_c = cur-height+1;
        }
        setDiagonalJumps({cur_r, cur_c}, kNorth+kEast);

        cur_r=0, cur_c=width-1;
        if (cur < height) {
            cur_r = cur;
        } else {
            cur_c = cur-height+1;
        }
        setDiagonalJumps({cur_r, cur_c}, kSouth+kWest);
    }
}

void Game::outResult() {
    for (int r=0; r<height; r++) {
        for (int c=0; c<width; c++) {
            if (getElem(map, {r, c}) != MapElem::kWall)
                cout << c << " " << r << getElem(jps_map, {r, c}) << endl;
        }
    }
}

bool Game::isAvailable(Point cur_point) {
    return !(cur_point.r < 0 || cur_point.r >= height || cur_point.c < 0 || cur_point.c >= width || getElem(map, cur_point) == MapElem::kWall);
}

int main()
{
    Game game = Game();
    cerr << "Start proceed" << endl;
    game.proceedMap();
    cerr << "Print result" << endl;
    game.outResult();
}