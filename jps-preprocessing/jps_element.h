#ifndef JPS_PREPROCESSING_JPS_ELEMENT_H
#define JPS_PREPROCESSING_JPS_ELEMENT_H

#include <vector>
#include <algorithm>
#include "point.h"

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
};

#endif //JPS_PREPROCESSING_JPS_ELEMENT_H
