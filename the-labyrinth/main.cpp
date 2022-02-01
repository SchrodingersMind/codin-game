#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

enum MapFields {
    kHollow,
    kWall,
    kSTart,
    kControl,
    kUnexplored
};

enum Moves {
    kUp,
    kRight,
    kDown,
    kLeft
};

// Point but for the grid with rows and columns (and not x and y)
class Point {
public:
    int r=-1, c=-1;

    bool isInitialized() const;
    int toInt();

    friend ostream& operator<<(ostream& stream, Point point);
};

bool Point::isInitialized() const {
    return r != -1;
}

ostream& operator<<(ostream& stream, Point point) {
    stream << "row: " << point.r << " col: " << point.c;
    return stream;
}

template<typename T>
int getElem(T map, Point point) {
    return map[point.r][point.c];
}
template<typename T>
void setElem(T map, Point point, int value) {
    map[point.r][point.c] = value;
}

class Game {
private:
    vector<vector<MapFields>> map;
    int **bfs_map;
    Point rick_pos{};
    Point control_pos{};
    int rows, cols;
    int end_time;
    vector<Moves> current_target;
    bool found_control_path=false;

    // Simple bfs implementation
    void findNearestUnexplored(MapFields target, bool with_end, Point start);
    vector<Moves> getSiblings(Point point);
    static Point getPointByMove(Point start, Moves move);
    int toInt(Point p) const;
    string moveToResult(Moves move);
    Moves getLast();
public:
    Game();

    void inputRound();
    string outputRound();
};

void Game::findNearestUnexplored(MapFields target, bool with_end, Point start) {
    queue<Point> to_burn;
    to_burn.push(start);
    setElem(bfs_map, start, 1);
    struct ToBacktrace{Point parent; Moves move=Moves::kUp;};
    auto* backtrace_arr = new ToBacktrace[rows*cols];
    Point end;
    while (!to_burn.empty() && !end.isInitialized()) {
        Point cur_point = to_burn.front();
        to_burn.pop();
        vector<Moves> possible_moves = getSiblings(cur_point);
        for (Moves move : possible_moves) {
            Point new_point = getPointByMove(cur_point, move);
            // Check if this point is "burned"
            if (getElem(bfs_map, new_point))
                continue;
            MapFields field = static_cast<MapFields>(getElem(map, new_point));
            if (field == target) {
                backtrace_arr[toInt(new_point)] = {cur_point, move};
                end = cur_point;
                if (with_end) {
                    end = new_point;
                }
                break;
            } else if (field == MapFields::kSTart || field == MapFields::kHollow) {
                to_burn.push(new_point);
                backtrace_arr[toInt(new_point)] = {cur_point, move};
            }
            setElem(bfs_map, new_point, 1);
        }

    }

    cerr << "End point is: " << end << endl;
    cerr << "Target size is: " << current_target.size() << endl;
    // Assume that we found needed path and now just backtracking
    ToBacktrace temp;
    while (end.isInitialized()) {
        temp = backtrace_arr[toInt(end)];
        current_target.push_back(temp.move);
        end = temp.parent;
    }
    if (!current_target.empty())
        current_target.pop_back();

    // Now clean up bfs map
    for (int r=0; r<rows; r++) {
        for (int c=0; c<cols; c++) {
            bfs_map[r][c] = 0;
        }
    }
}

vector<Moves> Game::getSiblings(Point point) {
    vector<Moves> result;
    if (point.r > 0)
        result.push_back(Moves::kUp);
    if (point.c < cols-1)
        result.push_back(Moves::kRight);
    if (point.r < rows-1)
        result.push_back(Moves::kDown);
    if (point.c > 0)
        result.push_back(Moves::kLeft);
    return result;
}

Point Game::getPointByMove(Point start, Moves move) {
    Point new_point;
    switch (move) {
        case Moves::kUp:
            new_point = Point{start.r-1, start.c};
            break;
        case Moves::kRight:
            new_point = Point{start.r, start.c+1};
            break;
        case Moves::kDown:
            new_point = Point{start.r+1, start.c};
            break;
        case Moves::kLeft:
            new_point = Point{start.r, start.c-1};
    }
    return new_point;
}

int Game::toInt(Point p) const {
    return p.r*cols+p.c;
}

Game::Game() {
    cin >> rows >> cols >> end_time; cin.ignore();
    map.resize(rows);
    bfs_map = new int*[rows];
    for (int i=0; i<rows; i++) {
        bfs_map[i] = new int[cols];
        for (int k=0; k<cols; k++)
            bfs_map[i][k] = 0;
    }
}

void Game::inputRound() {
    cin >> rick_pos.r >> rick_pos.c; cin.ignore();
    for (int i = 0; i < rows; i++) {
        string row; // C of the characters in '#.TC?' (i.e. one line of the ASCII maze).
        cin >> row; cin.ignore();
        map[i].clear();
        for (char c : row) {
            switch (c) {
                case '.':
                    map[i].push_back(MapFields::kHollow);
                    break;
                case '#':
                    map[i].push_back(MapFields::kWall);
                    break;
                case 'T':
                    map[i].push_back(MapFields::kSTart);
                    break;
                case 'C':
                    control_pos = Point{i, (int) map[i].size()};
                    map[i].push_back(MapFields::kControl);
                    break;
                case '?':
                    map[i].push_back(MapFields::kUnexplored);
                    break;
                default:
                    cerr << "You shouldn't be here..." << endl;
            }
        }
    }
}

Moves Game::getLast() {
    Moves result = current_target.back();
    current_target.pop_back();
    return result;
}

string Game::outputRound() {
    if (!current_target.empty()) {
        return moveToResult(getLast());
    }

    if (!control_pos.isInitialized()) {
        findNearestUnexplored(MapFields::kUnexplored, false, rick_pos);
        return moveToResult(getLast());
    } else {
        cerr << "Found control room!!!" << endl;
    }

    if (!found_control_path) {
        findNearestUnexplored(MapFields::kControl, true, rick_pos);
        if (current_target.empty()) {
            // Get control room in the map, but there is no visible path from rick to control room
            current_target.clear();
            findNearestUnexplored(MapFields::kUnexplored, false, rick_pos);
        } else {
            // There is a path, but we should check for the countdown time
            // (the time we have to fly from control room to start position)
            current_target.clear();
            findNearestUnexplored(MapFields::kSTart, true, control_pos);
            if (current_target.size() <= end_time) {
                // All good, go to the control room
                current_target.clear();
                found_control_path = true;
                findNearestUnexplored(MapFields::kControl, true, rick_pos);
            } else {
                // If time is not enough, explore new paths
                current_target.clear();
                findNearestUnexplored(MapFields::kUnexplored, false, rick_pos);
            }
        }
        return moveToResult(getLast());
    }

    findNearestUnexplored(MapFields::kSTart, true, rick_pos);
    return moveToResult(getLast());
}

string Game::moveToResult(Moves move) {
    string result;
    switch (move) {
        case Moves::kUp:
            result = "UP";
            break;
        case Moves::kRight:
            result = "RIGHT";
            break;
        case Moves::kDown:
            result = "DOWN";
            break;
        case Moves::kLeft:
            result = "LEFT";
            break;
    }
    return result;
}

int main()
{
    Game game = Game();

    while (1) {
        game.inputRound();
        cout << game.outputRound() << endl;
    }
}