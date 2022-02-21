#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <cmath>

#define PLAYER_ME 0

using namespace std;

enum Units {
    kReaper,
    kDestroyer,
    kDoof,
    kTanker,
    kWreck,
    kTar,
    kOil
};

class Point {
public:
    int x, y;

    double distance(Point p) const;
    bool isSharpAngle(Point vector_2) const;
    double len() const;
    Point operator+(Point other) const;
    Point operator-(Point other) const;
    Point operator/ (double div) const;
    bool operator== (Point other) const;
    bool operator!= (Point other) const;
    friend ostream& operator<<(ostream& stream, const Point& result);
};

double Point::distance(Point p) const {
    return sqrt(pow(x-p.x, 2) + pow(y-p.y, 2));
}

bool Point::isSharpAngle(Point vector_2) const {
    return (x*vector_2.x + y*vector_2.y > 0) || (x == 0 && y == 0) || (vector_2.x == 0 && vector_2.y == 0);
}

double Point::len() const {
    return sqrt(x*x + y*y);
}
Point Point::operator+(Point other) const {
    return Point{x+other.x, y+other.y};
}

Point Point::operator-(Point other) const {
    return Point {x-other.x, y-other.y};
}
Point Point::operator/(double div) const {
    return {(int)(x/div), (int)(y/div)};
}

bool Point::operator==(Point other) const {
    return (x == other.x && y == other.y);
}

bool Point::operator!=(Point other) const {
    return (x != other.x || y != other.y);
}
ostream& operator<<(ostream& stream, const Point& result) {
    stream << "x: " << result.x << " y: " << result.y;
    return stream;
}

enum SpecialMoves {
    kWait=-1,
    kSkill=-2
};

class MoveResult {
public:
    Point p={0, 0};
    int power=-1;
    Point true_target={0, 0};

//    MoveResult(): p{0, 0}, power{-1} {}
    friend ostream& operator<<(ostream& stream, const MoveResult& result);
};

ostream& operator<<(ostream& stream, const MoveResult& result) {
    if (result.power == SpecialMoves::kWait) {
        stream << "WAIT";
    } else if(result.power == SpecialMoves::kSkill) {
        stream << "SKILL " << result.p.x << " " << result.p.y;
    } else {
        stream << result.p.x << " " << result.p.y << " " << result.power;
    }
    return stream;
}

/////////////////////////////
////   UNIT BASE CLASS
/////////////////////////////
class Unit {
protected:
    int radius=0;
    double mass=0, friction=0;
    int player=-1;
public:
    int unit_type=0;
    Point target_mb{0, 0}, estimated{0, 0};
    Point p;
    Point v;

    Unit (int x, int y, int vx, int vy): p{x, y}, v{vx, vy} {}
    Unit (int x, int y, int vx, int vy, int mass, int radius, int ut, int player): p{x, y}, v{vx, vy},
        mass{(double)mass}, radius{radius}, unit_type{ut}, player{player} {}

    void estimate(Point p1, int power);
    void move();
    bool isMine() const;

    virtual int getRadius() { return 0; }
    virtual double getMass() { return 0.0; }
    virtual double getFriction() { return 0.0; }

    Point calculateStopPosition(Point target, int power) const;
    // Calculate power and point that needed to move by machine to reach current location
    MoveResult* calculateThrust(Point start, Point previous, double machine_mass) const;
    MoveResult* overseeMove();
};


void Unit::estimate(Point p1, int power) {
    double distance = p.distance(p1);
    double coef = (double)power / getMass() / distance;
    estimated.x = p.x+v.x+(int) ((p1.x - p.x) * coef);
    estimated.y = p.y+v.y+(int) ((p1.y - p.y) * coef);
    cerr << "Here estimated: " << estimated.x << " " << estimated.y << endl;
}

void Unit::move() {
    p.x += v.x*1;
    p.y += v.y*1;
}


bool Unit::isMine() const {
    return player == PLAYER_ME;
}
MoveResult* Unit::calculateThrust(Point start, Point v_previous, double machine_mass) const {
    // For first, calculate target point (it's vector from our machine position to the target)
    Point target{p.x - v_previous.x, p.y - v_previous.y};
    // Then calculate power that needed to reach this point
    double distance = start.distance(target);

    int power = 0;
    if (start.x == target.x) {
        if(start.y != target.y) {
            power = abs((int) (p.y-start.y-v_previous.y)*distance*machine_mass / (target.y-start.y));
        }
    } else {
            power = abs((int) (p.x-start.x-v_previous.x)*distance*machine_mass / (target.x-start.x));
    }
    return new MoveResult{target, power};
}

Point Unit::calculateStopPosition(Point target, int power) const {
    Point temp_v = v;
    double distance = p.distance(target);
    double coef = (double)power / mass / distance;
    temp_v.x += (int) ((target.x - p.x) * coef);
    temp_v.y += (int) ((target.y - p.y) * coef);
    return {p.x+temp_v.x, p.y+temp_v.y};
}

/////////////////////////////
////   BOTS
/////////////////////////////
class Wreck: public Unit {
protected:
    int radius;
public:
    bool is_blocked=false;
    virtual int getRadius() { return radius; }
    Wreck (int radius, int x, int y, int vx, int vy): Unit(x, y, vx, vy), radius{radius} {};

    bool isOverlappedBy(vector<Unit> some_skills);
};

bool Wreck::isOverlappedBy(vector<Unit> some_skills) {
    for (Unit skill : some_skills) {
        if (p.distance(skill.p) < abs(getRadius()-skill.getRadius())+50)
            return true;
    }
    return false;
}

class Tanker: public Unit {
protected:
    int radius;
public:
//    int end_x, end_y;
    int water;
    virtual int getRadius() { return radius; }
    Tanker (int radius, int x, int y, int vx, int vy, int water): Unit(x, y, vx, vy), radius{radius}, water{water} {
        p.x = x+vx;
        p.y = y+vy;
    };
};

/////////////////////////////
////   REAPER
/////////////////////////////
class Reaper: public Unit {
protected:
    int radius=400;
    float mass=0.5, friction=0.2;

    bool checkObstacles(const Wreck& target_wreck, const vector<Tanker>& tankers);
public:
    Reaper (int x, int y, int vx, int vy): Unit(x, y, vx, vy) {};

    virtual int getRadius() { return radius; }
    double getMass() override { return mass; }
    virtual double getFriction() { return friction; }

    MoveResult *findNearestWreck(vector<Wreck> wrecks, const vector<Tanker>& tankers, Point destroyer_position);
};

bool Reaper::checkObstacles(const Wreck& target_wreck, const vector<Tanker>& tankers) {
    if (target_wreck.is_blocked)
        return true;
    bool is_landed = false;
    for (const Tanker& tanker : tankers) {
        if (tanker.p.distance(target_wreck.p) < 500 && p.distance(tanker.p) < p.distance(target_wreck.p)+200)
            is_landed = true;
    }
    return is_landed;
}

MoveResult * Reaper::findNearestWreck(vector<Wreck> wrecks, const vector<Tanker>& tankers, Point destroyer_position) {
    if (wrecks.empty()) {
        estimate(destroyer_position, 300);
        target_mb = destroyer_position;
        return new MoveResult{destroyer_position, 300};
    }
    MoveResult* best = wrecks[0].calculateThrust(p, v, mass);
    MoveResult* temp;
    for (Wreck& wreck : wrecks) {
        temp = wreck.calculateThrust(p, v, mass);
        if (checkObstacles(wreck, tankers)) {
            cerr << "Shadowed wreck at " << wreck.p << endl;
            continue;
        }
//        cerr << "Estimated power: " << temp->power << endl;
        if (temp->power < best->power) {
            best = temp;
        }
    }
    if (best->power > 300) {
        best->power = 300;
    }
    target_mb = best->p;
    estimate(best->p, best->power);
    return best;
}

struct CalculateBestTanker {
    Tanker tanker;
    MoveResult* to_result{};
    int estimated_profit{};
};

/////////////////////////////
////   DESTROYER
/////////////////////////////
class Destroyer: public Unit {
protected:
    int radius=400;
    float mass=1.5, friction=0.3;

    virtual int getRadius() { return radius; }
    virtual double getMass() { return mass; }
    virtual double getFriction() { return friction; }

    static int calculateTankerProfit (Tanker tanker, Point reaper_position);
    static bool ToSortCalculates (CalculateBestTanker calc1, CalculateBestTanker calc2);
    static int calculateTankerProfit2 (Tanker tanker, Point reaper_position, int needed_power);
public:
    Destroyer (int x, int y, int vx, int vy): Unit(x, y, vx, vy) {};

    MoveResult* findNearestTank(vector<Tanker> tankers, Point reaper_position);
};

int Destroyer::calculateTankerProfit(Tanker tanker, Point reaper_position) {
    return (int)reaper_position.distance(tanker.p) - tanker.water*20;
}

int Destroyer::calculateTankerProfit2(Tanker tanker, Point reaper_position, int needed_power) {
    return (int) (tanker.water/reaper_position.distance(tanker.p));
//    return (int) (needed_power + reaper_position.distance(tanker.p) - tanker.water*40);
}

bool Destroyer::ToSortCalculates(CalculateBestTanker calc1, CalculateBestTanker calc2) {
    return calc1.estimated_profit < calc2.estimated_profit;
}

MoveResult *Destroyer::findNearestTank(vector<Tanker> tankers, Point reaper_position) {
    if (tankers.empty()) {
        return new MoveResult{{0, 0}, 300};
    }
    MoveResult* best = tankers[0].calculateThrust(p, v, mass);
    MoveResult* temp;
    vector<CalculateBestTanker> accessible;
    vector<CalculateBestTanker> all;
    for (Tanker& tanker : tankers) {
        if(tanker.p.len() > 6000) {
            // Tanker is out of map
            continue;
        }
        temp = tanker.calculateThrust(p, v, mass);
//        cerr << "Destroyer power: " << temp->power << endl;
        if (temp->power <= 300) {
            accessible.push_back({tanker, temp, calculateTankerProfit(tanker, reaper_position)});
        }
        all.push_back({tanker, temp, calculateTankerProfit2(tanker, reaper_position, temp->power)});
    }
    if (all.empty()) {
        return new MoveResult{{0, 0}, 300};
    }
    sort(all.begin(), all.end(), ToSortCalculates);
    best = all[0].to_result;

    if (best->power > 300) {
        best->power = 300;
    }
    return best;
}

/////////////////////////////
////   DOOFER
/////////////////////////////
class Doofer: public Unit {
protected:
    int radius=400;
    float mass=1, friction=0.25;

    static Wreck findClosestWreck(vector<Wreck> wrecks, Point target);
public:
    Doofer (int x, int y, int vx, int vy): Unit(x, y, vx, vy) {};

    virtual int getRadius() { return 400; }
    virtual double getMass() { return 1.0; }
    virtual double getFriction() { return 0.25; }

    MoveResult *findUncrowdedPath(vector<Unit> all_units, vector<Wreck> wrecks, Reaper *me, Reaper *target, int rage);
};

Wreck Doofer::findClosestWreck(vector<Wreck> wrecks, Point target) {
    Wreck closest = wrecks[0];
    double min = target.distance(wrecks[0].p);
    for (const Wreck& wreck : wrecks) {
        double tmp = target.distance(wreck.p);
        if (tmp < min) {
            min = tmp;
            closest = wreck;
        }
    }
    return closest;
}
MoveResult *Doofer::findUncrowdedPath(vector<Unit> all_units, vector<Wreck> wrecks, Reaper *me, Reaper *target, int rage) {
    // If rage is enough, just follow target and push and oil it
    if (rage > 150) {
        if (p.distance(target->p) > 3000) {
            return target->calculateThrust(p, v, getMass());
        }
        vector<Wreck> possible_targets;
        for (Wreck wreck : wrecks) {
            // Todo: me->target_mb doesnt represent actual "physical" target, instead it shows point near the target,
            // vector in which direction my reaper move
            if (target->v.isSharpAngle((wreck.p-target->p)) && target->p.distance(wreck.p) < wreck.getRadius()*2
                && (me->target_mb != wreck.p || me->p.distance(wreck.p) > 2000) && p.distance(wreck.p) < 3000-wreck.getRadius())
                possible_targets.push_back(wreck);
        }
        if (possible_targets.empty()) {
            return new MoveResult{target->p+target->v, 300};
        } else {
            Wreck closest = findClosestWreck(possible_targets, target->p);
            Point target_skill = closest.p;
            double dist = p.distance(target_skill);
            if (dist > 2000) {
                double coef = target_skill.len()/2000;
                target_skill = target_skill/coef;
            }
            cerr << "First doof-skill triggered" << endl;
            cerr << "Target " << me->target_mb << endl;
            return new MoveResult {target_skill, SpecialMoves::kSkill};
        }
    }
    // Check for the skill ability
    bool is_skill = true;
    if (rage > 90 && p.distance(me->p) > 4000 && p.distance(target->p) < 2400) {
        Point target_skill = target->p - p;
        if (target_skill.len() < 2000) {
            cerr << "Second doof-skill triggered" << endl;
            return new MoveResult{p+target_skill, SpecialMoves::kSkill};
        } else {
            cerr << "Third doof-skill triggered" << endl;
            double coef = target_skill.len()/2000;
            return new MoveResult{p+(target_skill/coef), SpecialMoves::kSkill};
        }
    }
    if (v.x == 0 && v.y == 0) {
        return new MoveResult{{0,0}, 300};
    }
    vector<Unit> second_check;
    for (Unit& unit : all_units) {
        if (v.isSharpAngle(unit.v) || p.distance(unit.p) > 2000 || (unit.p.x == p.x && unit.p.y == p.y)) {
            continue;
        }
        second_check.push_back(unit);
    }
    double gross_velocity = (300/mass / sqrt(v.x*v.x+v.y*v.y));
    // If we continue to move in the same direction....
    Point estimated_end = {p.x+v.x + (int)(gross_velocity*v.x), p.y+v.y + (int)(gross_velocity*v.y)};
    Point check_circle = (p+estimated_end)/2;
    double circle_len = p.distance(estimated_end)/2;
    enum Povorot{kTo_center, kTo_border, kBack_center, kBack_border};
    int in_circle = 0;
    int reapers=0;
    bool is_my_reaper=false;
    for (Unit& unit : second_check) {
        if (estimated_end.distance(unit.p) < 400+circle_len)
            in_circle++;
        if (unit.unit_type == Units::kReaper) {
            reapers++;
            if (unit.isMine())
                is_my_reaper  = true;
        }
    }
    if (in_circle > 2) {
//        if (reapers>0 && !is_my_reaper && rage > 30) {
//            return new MoveResult{check_circle, SpecialMoves::kSkill};
//        } else {
            return new MoveResult{{0, 0}, 300};
//        }
    } else {
        return new MoveResult{estimated_end, 300};
    }
}

class Player {
public:
    int score;
    int rage; // Unused
    Reaper *reaper;
    Destroyer *destroyer;
    Doofer *doofer;
};

/////////////////////////////
////   GAME
/////////////////////////////
class Game {
private:
    Player *me{};
    Player *opponent1{};
    Player *opponent2{};
    Player *target{};
    vector<Wreck> wrecks;
    vector<Tanker> tankers;
    vector<Unit> all_units;
    vector<Unit> oils;
    bool is_init = true;
public:
    void inputTurn();
    void answerTurn();
};

void Game::inputTurn() {
    int my_score;
    cin >> my_score; cin.ignore();
    int enemy_score_1;
    cin >> enemy_score_1; cin.ignore();
    int enemy_score_2;
    cin >> enemy_score_2; cin.ignore();
    int my_rage;
    cin >> my_rage; cin.ignore();
    int enemy_rage_1;
    cin >> enemy_rage_1; cin.ignore();
    int enemy_rage_2;
    cin >> enemy_rage_2; cin.ignore();

    me = new Player;
    me->score = my_score;
    me->rage = my_rage;
    opponent1 = new Player;
    opponent1->score = enemy_score_1;
    opponent2 = new Player;
    opponent2->score = enemy_score_2;
    target = opponent1;
    if (enemy_score_2 > enemy_score_1)
        target = opponent2;

    wrecks.clear();
    tankers.clear();
    all_units.clear();
    oils.clear();
    int unit_count;
    cin >> unit_count; cin.ignore();
    for (int i = 0; i < unit_count; i++) {
        int unit_id;
        int unit_type;
        int player;
        float mass;
        int radius;
        int x;
        int y;
        int vx;
        int vy;
        int extra;
        int extra_2;
        cin >> unit_id >> unit_type >> player >> mass >> radius >> x >> y >> vx >> vy >> extra >> extra_2; cin.ignore();
        all_units.emplace_back(x, y, vx, vy, mass, radius, unit_type, player);
        if (unit_type == Units::kWreck) {
            wrecks.emplace_back(radius, x, y, vx, vy);
//            cerr << "Added wreck: " << x << " " << y << endl;
        } else if (unit_type == Units::kTanker) {
            tankers.emplace_back(radius, x, y, vx, vy, extra);
//            Unit test_unit = Tanker{radius, x, y, vx, vy, extra};
        } else if (unit_type == Units::kOil) {
            oils.emplace_back(x, y, vx, vy, mass, radius, unit_type, player);
        }
        if (player == PLAYER_ME) {
            // Hello Parker, it's me
            if (unit_type == Units::kReaper) {
                if (me->reaper != nullptr) {
                }
                if (!is_init) {
                    cerr << "Real x, y " << x << " " << y << endl;
                }
                me->reaper = new Reaper{x, y, vx, vy};
            } else if (unit_type == Units::kDestroyer) {
                me->destroyer = new Destroyer{x, y, vx, vy};
            } else if (unit_type == Units::kDoof) {
                me->doofer = new Doofer{x, y, vx, vy};
            }
        } else if (player == 1) {
            if (unit_type == Units::kReaper) {
                opponent1->reaper = new Reaper{x, y, vx, vy};
            } else if (unit_type == Units::kDestroyer) {
                opponent1->destroyer = new Destroyer{x, y, vx, vy};
            } else if (unit_type == Units::kDoof) {
                opponent1->doofer = new Doofer{x, y, vx, vy};
            }
        } else if (player == 2) {
            if (unit_type == Units::kReaper) {
                opponent2->reaper = new Reaper{x, y, vx, vy};
            } else if (unit_type == Units::kDestroyer) {
                opponent2->destroyer = new Destroyer{x, y, vx, vy};
            } else if (unit_type == Units::kDoof) {
                opponent2->doofer = new Doofer{x, y, vx, vy};
            }
        }
    }
    for (Wreck& wreck : wrecks) {
        if (wreck.isOverlappedBy(oils)) {
            wreck.is_blocked = true;
        }
    }
    is_init = false;
}

void Game::answerTurn() {
    cout << *(me->reaper->findNearestWreck(wrecks, tankers, me->destroyer->p)) << endl;
    cout << *(me->destroyer->findNearestTank(tankers, me->reaper->p)) << endl;
    cout << *(me->doofer->findUncrowdedPath(all_units, wrecks, me->reaper, target->reaper, me->rage)) << endl;
}

int main()
{
    Game game{};
    // game loop
    while (1) {
        game.inputTurn();
        cerr << "Begin answering...." << endl;
        game.answerTurn();
    }
}