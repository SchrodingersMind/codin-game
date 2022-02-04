
#ifndef JPS_PREPROCESSING_POINT_H
#define JPS_PREPROCESSING_POINT_H

#include <vector>
#include <iostream>

using namespace std;

class Point {
public:
    int r, c;

    Point operator+ (Point other) const;
    Point operator- () const;
    Point operator- (Point other) const;
    void operator+= (Point other);
    bool operator== (Point other) const;
    friend ostream& operator<<(ostream& stream, Point point);
};

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

void Point::operator+=(Point other) {
    r += other.r;
    c += other.c;
}

bool Point::operator==(Point other) const {
    return r == other.r && c == other.c;
}

ostream& operator<<(ostream& stream, Point point) {
    stream << "row: " << point.r << " col: " << point.c;
    return stream;
}


Point kNorth = {-1, 0};
Point kEast = {0, 1};
Point kSouth = {1, 0};
Point kWest = {0, -1};

#endif //JPS_PREPROCESSING_POINT_H
