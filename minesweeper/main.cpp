#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

enum CellState {
    kEmpty,
    kUnknown,
    kNextToMine,
    kMine
};

class Point {
public:
    int row, col;
    Point(int row, int col): row(row), col(col) {};

    bool operator==(Point other) const;
    bool operator<(Point other) const;
    Point operator+ (Point other) const;
    friend ostream& operator<< (ostream& stream, Point other);
};

ostream& operator<< (ostream& stream, Point other) {
    stream << "r: " << other.row << " c: " << other.col;
    return stream;
}

bool Point::operator<(Point other) const {
    return this->row < other.row || (this->row == other.row && this->col < other.col);
}

Point Point::operator+(Point other) const {
    return {this->row+other.row, this->col+other.col};
}

bool Point::operator==(Point other) const {
    return this->row == other.row && this->col == other.col;
}

class Field {
public:
    CellState status = kUnknown;
    int mines = 0;
};

class Game {
private:
    vector<vector<Field>> map;
    vector<Point> mines;
    int height=16, width=30;
    bool is_init = true;

    void getNeighbors(Point p1, int &mines_count, vector<Point> &unknown, vector<Point> &next_to_mines);
    Field& getCell(Point p1);
    bool cellToReveal(int &target_row, int &target_col);
    void findLessPossibleCell(int &target_row, int &target_col);
    // Try to brute-force count of the successful map combinations with current cell marked as mine
    // "probability" vector represent this count (there is no need in division, because number of maps without errors same in all cases)
    // suspect_ptr represent current cell position in suspects array and its successful count in "probabilities"
    int recursiveGetProbabilities(vector<Point> &suspects, int suspect_ptr, vector<int> &probabilities);
public:
    Game() {};

    void inputRound();
    string outputRound();
};

inline Field& Game::getCell(Point p1) {
    return map[p1.row][p1.col];
}

void Game::getNeighbors(Point p1, int &mines_count, vector<Point> &unknown, vector<Point> &next_to_mines) {
    vector<Point> result;
    mines_count = 0;
    for (int r=-1; r<2; r++) {
        for (int c=-1; c<2; c++) {
            if (r == 0 && c == 0)
                continue;
            if (p1.row+r>=0 && p1.row+r<height && p1.col+c>=0 && p1.col+c<width) {
                Point cur_point = p1 + Point{r, c};
                if (getCell(cur_point).status == kUnknown)
                    unknown.push_back(cur_point);
                else if (getCell(cur_point).status == kNextToMine)
                    next_to_mines.push_back(cur_point);
                else if (getCell(cur_point).status == kMine)
                    mines_count++;
            }
        }
    }
}

bool Game::cellToReveal(int &target_row, int &target_col) {
    for (int r=0; r<height; r++) {
        for (int c=0; c<width; c++) {
            Point cur{r, c};
            Field cur_cell = getCell(cur);
            if (cur_cell.status != kNextToMine)
                continue;
            int mines_count = 0;
            vector<Point> unknown;
            vector<Point> next_to_mines;
            getNeighbors(cur, mines_count, unknown, next_to_mines);
            if (unknown.empty()) {
                continue;
            }
            if (mines_count+unknown.size() == cur_cell.mines) {
                // Mark all cells around here as mines
                cerr << "Mark all as mines" << endl;
                mines.insert(mines.end(), unknown.begin(), unknown.end());
                for (Point neighbor : unknown) {
                    getCell(neighbor).status = kMine;
                }
            } else if (mines_count == cur_cell.mines) {
                // Mark all cells around here as non-mines
                cerr << "Found target" << endl;
                target_row = unknown[0].row;
                target_col = unknown[0].col;
                return true;
            } else if (!next_to_mines.empty()) {
                // Now should be check for the
                for (Point num_neighbor : next_to_mines) {
                    vector<Point> tmp_unknown;
                    vector<Point> tmp_to_mines;
                    int tmp_mines_count;
                    getNeighbors(num_neighbor, tmp_mines_count, tmp_unknown, tmp_to_mines);
                    // Find unknown cells that adjacent only to second cell
                    int adjacent_count = 0;
                    vector<Point> not_adjacent;
                    for (Point old_neighbor: unknown) {
                        if (find(tmp_unknown.begin(), tmp_unknown.end(), old_neighbor) == tmp_unknown.end())
                            not_adjacent.push_back(old_neighbor);
                        else
                            adjacent_count++;
                    }
                    int old_unexploded = cur_cell.mines-mines_count;
                    int new_unexploded = getCell(num_neighbor).mines-tmp_mines_count;
                    if (!not_adjacent.empty() && old_unexploded != 0 && new_unexploded-(tmp_unknown.size()-adjacent_count) == old_unexploded) {
                        cerr << "Found using complicated method" << endl;
                        cerr << "Between point " << cur << " and " << num_neighbor << endl;
                        cerr << cur_cell.mines << " " << mines_count << " " << not_adjacent.size() << endl;
                        cerr << getCell(num_neighbor).mines << " " << tmp_mines_count << tmp_unknown.size() << adjacent_count << endl;

                        // So, mine is somewhere in between cur and num_neighbor
                        // And others is clear
                        target_row = not_adjacent[0].row;
                        target_col = not_adjacent[0].col;
                        return true;
                    } else if (!not_adjacent.empty() && old_unexploded != 0 && old_unexploded-new_unexploded == not_adjacent.size()) {
                        cerr << "Found hard way to set a mine flag!!!!" << endl;
                        mines.insert(mines.end(), not_adjacent.begin(), not_adjacent.end());
                        for (Point neighbor : not_adjacent) {
                            getCell(neighbor).status = kMine;
                        }
                    }
                }
            }
        }
    }
    return false;
}

int Game::recursiveGetProbabilities(vector<Point> &suspects, int suspect_ptr, vector<int> &probabilities) {
    if (suspects.size() == suspect_ptr)
        return 1;
    int successful = 0;
    Point cur_p = suspects[suspect_ptr];

    // Test if it is mined
    getCell(cur_p).status = kMine;
    int mines_count = 0;
    vector<Point> unknown;
    vector<Point> next_to_mines;
    // Only care about next_to_mines
    getNeighbors(cur_p, mines_count, unknown, next_to_mines);
    bool is_success = true;
    for (Point numbered : next_to_mines) {
        mines_count = 0;
        unknown.clear();
        vector<Point> other_numbered;
        // Only care about next_to_mines
        getNeighbors(numbered, mines_count, unknown, other_numbered);
        if (getCell(numbered).mines < mines_count) {
            is_success = false;
            break;
//            cerr << "Here add " << successful << endl;
        }
    }
    if (is_success)
        successful += recursiveGetProbabilities(suspects, suspect_ptr+1, probabilities);
    probabilities[suspect_ptr] += successful;

    // Now test if cell could be empty
    getCell(cur_p).status = kEmpty;
//    getNeighbors(cur_p, mines_count, unknown, next_to_mines);
    is_success = true;
    for (Point numbered : next_to_mines) {
        mines_count = 0;
        unknown.clear();
        vector<Point> other_numbered;
        // Only care about next_to_mines
        getNeighbors(numbered, mines_count, unknown, other_numbered);
        if (getCell(numbered).mines-mines_count > unknown.size()) {
            is_success = false;
            break;
//            cerr << "Add empty " << successful << endl;
        }
    }
    if (is_success)
        successful += recursiveGetProbabilities(suspects, suspect_ptr+1, probabilities);

    // Clean-up
    getCell(cur_p).status = kUnknown;
    return successful;
}

void Game::findLessPossibleCell(int &target_row, int &target_col) {
    // Find the probability to contain mine for each unknown cell, adjacent to numbered
    // Return cell with the lowest probability

    set<Point> borders;
    for (int r=0; r<height; r++) {
        for (int c=0; c<width; c++) {
            Point cur_p{r, c};
            if (getCell(cur_p).status != kNextToMine)
                continue;
            int mines_count = 0;
            vector<Point> unknown;
            vector<Point> next_to_mines;
            // Only care about next_to_mines
            getNeighbors(cur_p, mines_count, unknown, next_to_mines);
            borders.insert(unknown.begin(), unknown.end());
        }
    }

    vector<Point> suspects(borders.begin(), borders.end());
    vector<int> probabilities(suspects.size(), 0);
    recursiveGetProbabilities(suspects, 0, probabilities);
    int min_probability=INT32_MAX, min_index=0;
    for (int i=0; i<suspects.size(); i++) {
        if (probabilities[i] < min_probability) {
            min_probability = probabilities[i];
            min_index = i;
        }
        cerr << "Border elem " << suspects[i] << " with probability: " << probabilities[i] << endl;
    }
    target_row = suspects[min_index].row;
    target_col = suspects[min_index].col;
}

void Game::inputRound() {
    map.clear();
    map.resize(height);
    for (int i = 0; i < height; i++) {
        map[i].resize(width);
        for (int j = 0; j < width; j++) {
            string cell; // '?' if unknown, '.' if no mines nearby, '1'-'8' otherwise
            cin >> cell; cin.ignore();
            bool is_mine = false;
            for (Point mine_p : mines) {
                if (mine_p == Point{i, j}) {
                    is_mine = true;
                    break;
                }
            }
            if (is_mine) {
                map[i][j] = Field{kMine};
            } else if (cell == "?") {
                map[i][j] = Field{kUnknown};
            } else if (cell == ".") {
                map[i][j] = Field{kEmpty};
            } else {
                map[i][j] = Field{kNextToMine, stoi(cell)};
            }
        }
    }
}

string Game::outputRound() {
    if (is_init) {
        is_init = false;
        // Tap anywhere
        return "20 7";
    }
    string result;
    int target_row, target_col;
    if (!cellToReveal(target_row, target_col)) {
        cerr << "Start new loop" << endl;
        if (!cellToReveal(target_row, target_col)) {
            cerr << "I don't know what to do, maybe throw error?" << endl;
//            cout << "Never gonna give you up" << endl;
            findLessPossibleCell(target_row, target_col);
        }
    }

    for (Point mine: mines) {
        result += " " + to_string(mine.col) + " " + to_string(mine.row);
    }
    return to_string(target_col) + " " + to_string(target_row) + result;
}

int main()
{
    Game game;
    // game loop
    while (1) {
        game.inputRound();
        cout << game.outputRound() << endl;
    }
}