/////////     File: main.cpp     /////////
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <cmath>
#include <iomanip>
//#include <qheap.h>

/////////     File: point.h     /////////

#ifndef JPS_PREPROCESSING_POINT_H
#define JPS_PREPROCESSING_POINT_H

#include <vector>
#include <iostream>

using namespace std;

class Point {
public:
    Point(int row, int col);
    Point(): r{0}, c{0} {};

    int r, c;

    bool isCardinal() const;
    bool inDirection(Point& dir);

    virtual Point operator+ (Point other) const;
    virtual Point operator- () const;
    virtual Point operator- (Point other) const;
    Point operator* (int other) const;
    Point operator* (Point other) const;
    void operator+= (Point other);
    bool operator== (Point other) const;
    friend ostream& operator<<(ostream& stream, Point point);
};

bool Point::isCardinal() const {
    return r == 0 || c == 0;
}

bool Point::inDirection(Point& dir) {
    Point test = {this->r, this->c};
    if (test.r < 0) {
        test.r = -1;
    } else if (test.r > 0) {
        test.r = 1;
    } else {
        test.r = 0;
    }
    if (test.c < 0) {
        test.c = -1;
    } else if (test.c > 0) {
        test.c = 1;
    } else {
        test.c = 0;
    }
    return test == dir;
}

Point Point::operator+(Point other) const {
    Point second = other;
    second.r += this->r;
    second.c += this->c;
    return second;
}

Point Point::operator-() const {
    Point new_p{-this->r, -this->c};
    return new_p;
}

Point Point::operator-(Point other) const {
    Point new_p{this->r-other.r, this->c-other.c};
    return new_p;
}

Point Point::operator*(int other) const {
    Point new_p{this->r*other, this->c*other};
    return new_p;
}

Point Point::operator*(Point other) const {
    Point new_p{this->r*other.r, this->c*other.c};
    return new_p;
}

void Point::operator+=(Point other) {
    r += other.r;
    c += other.c;
}

bool Point::operator==(Point other) const {
    return r == other.r && c == other.c;
}

Point::Point(int row, int col): r{row}, c{col} {
}

ostream& operator<<(ostream& stream, Point point) {
    stream << "row: " << point.r << " col: " << point.c;
    return stream;
}


Point kNorth = {-1, 0};
Point kEast = {0, 1};
Point kSouth = {1, 0};
Point kWest = {0, -1};

class ExtendedPoint{
public:
    Point pos{0, 0};
    double passed=0, estimated=0;
    Point direction;
    ExtendedPoint* parent = nullptr;

    ExtendedPoint() = default;;
    ExtendedPoint(Point p1);
    ExtendedPoint(int row, int col) {this->pos=Point{row, col};};

//    bool inDirection(Point& direction);
//    bool operator== (ExtendedPoint other) const;
    bool operator< (ExtendedPoint other) const;
//    ExtendedPoint operator+ (ExtendedPoint other) const;
//    ExtendedPoint operator- (ExtendedPoint other) const;
//    ExtendedPoint operator* (ExtendedPoint other) const;
};

ExtendedPoint::ExtendedPoint(Point p1) {
    this->pos = p1;
}

bool ExtendedPoint::operator<(ExtendedPoint other) const {
    return this->estimated+this->passed < other.estimated+other.passed;
}
//bool ExtendedPoint::operator==(ExtendedPoint other) const {
//    return r == other.r && c == other.c;
//}

//ExtendedPoint ExtendedPoint::operator+(ExtendedPoint other) const {
//    ExtendedPoint result{this->pos.r+other.pos.r, this->c.pos+other.c};
//    return result;
//}
//
//ExtendedPoint ExtendedPoint::operator-(ExtendedPoint other) const {
//    ExtendedPoint result{this->r-other.r, this->c-other.c};
//    return result;
//}
//
//ExtendedPoint ExtendedPoint::operator*(ExtendedPoint other) const {
//    ExtendedPoint result{this->r*other.r, this->c*other.c};
//    return result;
//}


#endif //JPS_PREPROCESSING_POINT_H
/////////     End: point.h     /////////

/////////     File: jps_element.h     /////////
#ifndef JPS_PREPROCESSING_JPS_ELEMENT_H
#define JPS_PREPROCESSING_JPS_ELEMENT_H

#include <vector>
#include <algorithm>

using namespace std;

class JPSElem {
private:
    static int getPosition(Point direction);
public:
    vector<Point> jumpPoints;
    vector<int> distance;

    JPSElem();

    void setDistance(Point direction, int value);
    int getDistance(Point direction);
    void addJumpPoint(Point direction1, Point direction2);
    bool isJumpPoint(Point direction);
    friend ostream& operator<< (ostream& stream, JPSElem elem);
    friend istream& operator>> (istream& stream, JPSElem& elem);
};

#endif //JPS_PREPROCESSING_JPS_ELEMENT_H
/////////     End: jps_element.h     /////////


using namespace std;

/*
 * Traveling South:     West, Southwest, South, Southeast, East
 * Traveling Southeast:    South, Southeast, East
 * Traveling East:         South, Southeast, East, Northeast, North
 * Traveling Northeast:    East, Northeast, North
 * Traveling North:        East, Northeast, North, Northwest, West
 * Traveling Northwest:    North, Northwest, West
 * Traveling West:         North, Northwest, West, Southwest, South
 * Traveling Southwest:    West, Southwest, South
 * North - direction.r == -1
 * East - direction.c == 1
 * South - direction.r == 1
 * West - direction.c == -1
 */
vector<Point> getInertiaDirections(const Point& direction) {
    vector<Point> result;
    if (direction.r == 0 && direction.c == 0) {
        return {kNorth, kNorth+kEast, kEast, kSouth+kEast, kSouth, kSouth+kWest, kWest, kNorth+kWest};
    }
    if ((int) (abs(direction.r)+abs(direction.c)) == 2) {
        if (direction.r == direction.c) {
            result.emplace_back(direction.r, 0);
            result.emplace_back(direction.r, direction.c);
            result.emplace_back(0, direction.c);
        } else {
            result.emplace_back(0, direction.c);
            result.emplace_back(direction.r, direction.c);
            result.emplace_back(direction.r, 0);
        }
    } else if (direction.r != 0) {
        result.emplace_back(0, -direction.r);
        result.emplace_back(direction.r, -direction.r);
        result.emplace_back(direction.r, 0);
        result.emplace_back(direction.r, direction.r);
        result.emplace_back(0, direction.r);
    } else {
        result.emplace_back(direction.c, 0);
        result.emplace_back(direction.c, direction.c);
        result.emplace_back(0, direction.c);
        result.emplace_back(-direction.c, direction.c);
        result.emplace_back(-direction.c, 0);
    }
    if (direction.r == 0 && direction.c == 0) {
        cerr << "Found invalid direction in getInertiaDirections!!!" << endl;
    }
    for (const Point& in_res : result) {
        if (in_res.r == 0 && in_res.c == 0) {
            cerr << "Found invalid result in getInertiaDirections!!!" << endl;
        }
    }
    return result;
}

void printSet(const multiset<ExtendedPoint>& test_set) {
//    cerr << "\nPrint test_set" << endl;
    cerr << "\t";
    std::for_each(test_set.begin(), test_set.end(), [](const ExtendedPoint& x) {
        cerr << x.pos << " ";
    });
    cerr << endl;
    cerr << endl;
}

double estimate(Point p1, Point p2) {
    int dr = abs(p1.r - p2.r);
    int dc = abs(p1.c - p2.c);
    return dr + dc + (sqrt(2)-2) * min(dr, dc);
}

template <typename T>
T& getElem(vector<vector<T>>& some_map, ExtendedPoint pos) {
    return some_map[pos.pos.r][pos.pos.c];
}

class Game {
private:
    int height, width;
    vector<vector<JPSElem>> jps_map;
    Point start{0, 0};
    Point end{0, 0};
    bool found_path = false;
    vector<ExtendedPoint> points_passed;
public:
    Game();

    void proceedJPS();
    void printOutput();
};

Game::Game() {
    cin >> width >> height; cin.ignore();
    jps_map.resize(height);
    for (int r=0; r<height; r++) {
        jps_map[r].resize(width);
    }

    cin >> start.c >> start.r >> end.c >> end.r; cin.ignore();
    int open; // number of open tiles on the map
    cin >> open; cin.ignore();
    for (int i = 0; i < open; i++) {
        int column; // coordinate of the empty tile described
        int row; // coordinate of the empty tile described
        JPSElem new_elem;
        cin >> column >> row >> new_elem; cin.ignore();
        jps_map[row][column] = new_elem;
    }
}

void Game::proceedJPS() {
    multiset<ExtendedPoint> closed_list;
    multiset<ExtendedPoint> open_list;
    open_list.insert(start);
    while (!open_list.empty() && !found_path) {
        ExtendedPoint cur_p = *open_list.begin();
        open_list.erase(open_list.begin());
        closed_list.insert(cur_p);
        Point to_end = end-cur_p.pos;
        points_passed.push_back(cur_p);
        cerr << "Processing " << cur_p.pos << " and to end " << to_end << endl;

        if (cur_p.pos == end) {
            cerr << "You-hoo found end point" << endl;
            found_path = true;
            return;
        }
        JPSElem cur_elem = getElem(jps_map, cur_p);
        for (Point new_direct : getInertiaDirections(cur_p.direction)) {
//            cerr << "\tNew direction is: " << new_direct << endl;
            int to_pass_distance = cur_elem.getDistance(new_direct);
            ExtendedPoint new_successor;
//            cerr << to_pass_distance << endl;
//            cerr << new_direct.isCardinal() << " " << to_end.inDirection(new_direct) << " " << (abs(to_end.r+to_end.c) <= abs(to_pass_distance)) << endl;
            if (new_direct.isCardinal() && to_end.inDirection(new_direct) && abs(to_end.r+to_end.c) <= abs(to_pass_distance)) {
                cerr << "\tFirst if passed" << endl;
                new_successor = this->end;
                new_successor.passed = cur_p.passed+abs(to_end.r+to_end.c);
            } else if (!new_direct.isCardinal() && to_end.inDirection(new_direct) &&
                    (abs(to_end.r) <= abs(to_pass_distance) ||
                    abs(to_end.c) <= abs(to_pass_distance))) {
                cerr << "\tSecond if passed" << endl;
                int min_diff = min(abs(to_end.r), abs(to_end.c));
                new_successor = cur_p.pos + Point{min_diff, min_diff}*new_direct;
                new_successor.passed = cur_p.passed + (sqrt(2)*min_diff);
            } else if (to_pass_distance > 0) {
                cerr << "\tThird if passed" << endl;
                new_successor = cur_p.pos + new_direct*abs(to_pass_distance);
                new_successor.passed = abs(to_pass_distance);
                if (!new_direct.isCardinal())
                    new_successor.passed *= sqrt(2);
                new_successor.passed += cur_p.passed;
            }

            // Here should be traditional A*
            if (new_successor.passed != 0) {
                cerr << "\tAppend new successor " << new_successor.pos << endl;
                auto closed_it = find_if(closed_list.begin(), closed_list.end(), [&new_successor](const ExtendedPoint& cmp_point){
                    return cmp_point.pos == new_successor.pos;
                });
                if (closed_it == closed_list.end()) {
                    new_successor.estimated = estimate(new_successor.pos, end);
//                    auto mb_old = open_list.find(new_successor);
                    auto mb_old = find_if(open_list.begin(), open_list.end(), [&new_successor](const ExtendedPoint& cmp_point){
                        return cmp_point.pos == new_successor.pos;
                    });
                    if (mb_old == open_list.end() || new_successor < *mb_old) {
                        cerr << "\tActually append" << endl;
                        new_successor.parent = new ExtendedPoint(cur_p);
                        new_successor.direction = new_direct;
                        if (mb_old != open_list.end()) {
                            cerr << "Erase previous " << mb_old->pos << " to " << new_successor.pos << endl;
                            open_list.erase(mb_old);
                        }
                        open_list.insert(new_successor);
//                        printSet(open_list);
                    }
                }
            }
        }
        cerr << "\n\tOpenList size is: " << open_list.size() << endl;
        printSet(open_list);
        cerr << "\n\tPrint closed list" << endl;
        printSet(closed_list);
    }
}

void Game::printOutput() {
    cout << start.c << " " << start.r << " -1 -1 0.00" << endl;
    for (auto it=points_passed.begin()+1; it!=points_passed.end(); it++) {
        cerr << "Before print" << endl;
        cout << it->pos.c << " " << it->pos.r << " " << it->parent->pos.c << " " << it->parent->pos.r << " " << setprecision(2) << fixed << it->passed << endl;
        cerr << "After print" << endl;
    }
    if (!found_path)
        cout << "NO PATH" << endl;
}

int main()
{
    // In order of nodes visited by the JPS+ algorithm, a line containing "nodeColumn nodeRow parentColumn parentRow givenCost".
//    cout << "startColumn startRow -1 -1 0.00" << endl;
    Game game = Game();
    // game loop
    game.proceedJPS();
    game.printOutput();
}
/////////     End: main.cpp     /////////

/////////     File: jps_element.cpp     /////////


int JPSElem::getPosition(Point direction) {
    if (direction == kNorth)
        return 0;
    else if (direction == kNorth+kEast)
        return 1;
    else if (direction == kEast)
        return 2;
    else if (direction == kSouth+kEast)
        return 3;
    else if (direction == kSouth)
        return 4;
    else if (direction == kSouth+kWest)
        return 5;
    else if (direction == kWest)
        return 6;
    else if (direction == kWest+kNorth)
        return 7;
    cerr << "Invalid direction type in JPSElem::getPosition " << direction << endl;
    return 8;
}

JPSElem::JPSElem() {
    distance = vector<int>(8, 0);
}

void JPSElem::setDistance(Point direction, int value) {
    int pos = getPosition(direction);
    distance[pos] = value;
}

int JPSElem::getDistance(Point direction) {
    int pos = getPosition(direction);
    return distance[pos];
}

void JPSElem::addJumpPoint(Point direction1, Point direction2) {
    jumpPoints.push_back(direction1);
    jumpPoints.push_back(direction2);
}

bool JPSElem::isJumpPoint(Point direction) {
    auto it = find(jumpPoints.begin(), jumpPoints.end(), direction);
    return it != jumpPoints.end();
}


ostream& operator<<(ostream& stream, JPSElem elem) {
//    stream << "row: " << point.r << " col: " << point.c;
    for (int i=0; i<8; i++) {
        stream << " " << elem.distance[i];
    }
    return stream;
}

istream& operator>> (istream& stream, JPSElem& elem) {
    for (int i=0; i<8; i++) {
        stream >> elem.distance[i];
    }
    return stream;
}
/////////     End: jps_element.cpp     /////////

