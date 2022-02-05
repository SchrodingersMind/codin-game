
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
