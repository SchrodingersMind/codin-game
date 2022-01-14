#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

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
    double velocity;
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

int getShutterLandy() {
    for (int i=0; i<surface_n; i++) {
        if (land[i].x > shuttle.pos.x) {
            if (i != 0)
                return land[i-1].y;
        }
    }
    return land[surface_n-1].y;
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
    }

    bool start_acceleration = false;
    int y_land = 0;
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
        shuttle.velocity = v_speed;

        if (y_land == 0) {
            y_land = getShutterLandy();
        }


        // 2 integers: rotate power. rotate is the desired rotation angle (should be 0 for level 1), power is the desired thrust power (0 to 4).
        if (start_acceleration){
            cout << "0 4" << endl;
        } else {
            start_acceleration = isNeedToStartAGas(shuttle.pos.y-y_land, v_speed);
            cout << "0 0" << endl;
        }
    }
}