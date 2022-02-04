
#include "jps_element.h"

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