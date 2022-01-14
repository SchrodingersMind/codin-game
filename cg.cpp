#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

enum ShuttleStates {
    GoToFlatLand,
    SetHSpeedToZero,
    FreeLanding,
    SetVSpeedToZero
};

enum Rotates {
    RotateLeft,
    RotateRight,
    RotateSlightLeft,
    RotateSlightRight,
    NextState
};

class Point{
public:
    int x;
    int y;

};

Point* land;
int surface_n; // the number of points used to draw the surface of Mars.

class Shuttle{
public:
    int power;
    double v_velocity;
    double h_velosity;
    double angle;
    Point pos;
};

Shuttle shuttle;

bool isNeedToStartAGas(double to_ground, double speed) {
    // Note: speed should be negative, when to_ground - positive
    // Assume that land speed should be zero
    // Than 0 = speed + 0.289*t - 0.711 - 1.1711 - 2.711
    // (Assume that we switch from  zero power to full and then land in full)
    // Find time needed to set speed to the zero
    // And check if this time is enough to cross the distance between shuttle and ground
    double t = (5.133-speed)/0.289;
    cerr << "t = " << t << endl;
    double assumed_length = -(speed*3 - 9.6995 + (speed-5.133)*t + 0.1445*t*t);
    cerr << "assumed length = " << assumed_length << endl;
    if (assumed_length > to_ground) {
        return true;
    }
    return false;
}

double getHorizontalLength() {
    // For first, get time in second needed for the shuttle to fully drop horizontal speed
    // Then find the length, that needed to cross before this happens

    // With standard -45 degree and 3 power change in speed is +-2m/s per round (and 4 extra rounds for rotating to opposite side)
    double t = -shuttle.h_velosity/2.828;
    double length = abs(shuttle.h_velosity*t) - abs(1.414*t*t);
    cerr << "Length to stop horizontal is: " << length << endl;
    return length;
}

int goToFlatGround(Point start_flat, Point end_flat) {
    if (start_flat.x > shuttle.pos.x && start_flat.x>shuttle.pos.x+getHorizontalLength()) {
        if (abs(shuttle.h_velosity) > 80)
            return Rotates::RotateSlightRight;
        else
            return Rotates::RotateRight;
    } else if (end_flat.x < shuttle.pos.x-getHorizontalLength()) {
        if (abs(shuttle.h_velosity) > 80)
            return Rotates::RotateSlightLeft;
        else
            return Rotates::RotateLeft;
    } else
        return Rotates::NextState;
}

int makeShuttleStraight() {
    if (abs(shuttle.h_velosity) > 2) {
        if (shuttle.h_velosity < 0)
            return Rotates::RotateRight;
        else
            return Rotates::RotateLeft;
//    } else if (abs(shuttle.h_velosity) >= 1) {
//        if (shuttle.h_velosity < 0)
//            return RotateSlightRight;
//        else
//            return RotateSlightLeft;
    }
    return Rotates::NextState;
}

void getFlatGround(Point *start, Point *end) {
    int prev_y = land[0].y;
    for (int i=1; i<surface_n; i++) {
        if (prev_y == land[i].y) {
            start->x = land[i-1].x;
            start->y = land[i-1].y;
            end->x = land[i].x;
            end->y = land[i].y;
            if (prev_y == land[i+1].y) {
                cerr << "Found next flat ground!";
            }
            break;
        }
        prev_y = land[i].y;
    }
    cerr << "Found start_point " << start->x << " " << start->y << endl;
}

int main()
{
    cin >> surface_n; cin.ignore();
    land = new Point[surface_n];
    for (int i = 0; i < surface_n; i++) {
        int land_x; // X coordinate of a surface point. (0 to 6999)
        int land_y; // Y coordinate of a surface point. By linking all the points together in a sequential fashion, you form the surface of Mars.
        cin >> land_x >> land_y; cin.ignore();
        land[i] = Point{land_x, land_y};
        cerr << "Got point: " << land[i].x << " " << land[i].y << endl;
    }
    Point start_flat, end_flat;
    getFlatGround(&start_flat, &end_flat);

    bool high_ground = start_flat.y>1000;
    int high_speed_i = 40;
    int shuttle_state = ShuttleStates::GoToFlatLand;
    // game loop
    while (1) {
        int x;
        int y;
        int h_speed; // the horizontal speed (in m/s), can be negative.
        int v_speed; // the vertical speed (in m/s), can be negative.
        int fuel; // the quantity of remaining fuel in liters.
        int rotate; // the rotation angle in degrees (-90 to 90).
        int power; // the thrust power (0 to 4).
        cin >> x >> y >> h_speed >> v_speed >> fuel >> rotate >> power; cin.ignore();
        shuttle.pos = Point {x, y};
        shuttle.power = power;
        shuttle.v_velocity = v_speed;
        shuttle.h_velosity = h_speed;
        shuttle.angle = rotate;
        int func_result;

        if (high_ground) {
            if (high_speed_i > 0) {
                cout << "0 4" << endl;
                high_speed_i--;
                continue;
            }
        }
        switch(shuttle_state) {
            case (ShuttleStates::GoToFlatLand):
                func_result = goToFlatGround(start_flat, end_flat);
                if (func_result == Rotates::RotateLeft) {
                    cout << "30 4" << endl;
                } else if (func_result == Rotates::RotateSlightLeft) {
                    cout << "15 4" << endl;
                } else if (func_result == Rotates::RotateRight) {
                    cout << "-30 4" << endl;
                } else if (func_result == Rotates::RotateSlightRight) {
                    cout << "-15 4" << endl;
                } else if (func_result == Rotates::NextState) {
                    cout << "0 0" << endl;
                    cerr << "Now try to set horizontal speed to zero" << endl;
                    shuttle_state = ShuttleStates::SetHSpeedToZero;
                }
                break;
            case (ShuttleStates::SetHSpeedToZero):
                func_result = makeShuttleStraight();
                if (func_result == Rotates::RotateLeft) {
                    cout << "45 4" << endl;
                } else if (func_result == Rotates::RotateSlightLeft) {
                    cout << "15 3" << endl;
                } else if (func_result == Rotates::RotateRight) {
                    cout << "-45 4" << endl;
                } else if (func_result == Rotates::RotateSlightRight) {
                    cout << "-15 3" << endl;
                } else if (func_result == Rotates::NextState) {
                    cout << "0 2" << endl;
                    cerr << "Now try to free fall" << endl;
                    shuttle_state = ShuttleStates::FreeLanding;
                }
                break;
            case (ShuttleStates::FreeLanding):
                if (isNeedToStartAGas(shuttle.pos.y-start_flat.y, v_speed))
                    shuttle_state = ShuttleStates::SetVSpeedToZero;
                cout << "0 2" << endl;
                break;
            case (ShuttleStates::SetVSpeedToZero):
                cout << "0 4" << endl;
                break;
        }

        if (shuttle.pos.y < 0)
            break;
    }
}