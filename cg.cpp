#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

using namespace std;

enum PlayerID_e {
    kNone = -1,
    kMe = 0,
    kOpponent = 1
};

class Vitamin{
public:
    Vitamin(int v_a, int v_b, int v_c, int v_d, int v_e);

    int a, b, c, d, e;
    Vitamin(Vitamin const &copied) {
        a = copied.a;
        b = copied.b;
        c = copied.c;
        d = copied.d;
        e = copied.e;
    }
    string getNeeded(const Vitamin& fulfill) const;
    int summary() const;
};

string Vitamin::getNeeded(const Vitamin& fulfill) const {
    if (a < fulfill.a)
        return "A";
    if (b < fulfill.b)
        return "B";
    if (c < fulfill.c)
        return "C";
    if (d < fulfill.d)
        return "D";
    if (e < fulfill.e)
        return "E";
    return "";
}

int Vitamin::summary() const {
    return a+b+c+d+e;
}

Vitamin::Vitamin(int v_a, int v_b, int v_c, int v_d, int v_e) {
    a = v_a;
    b = v_b;
    c = v_c;
    d = v_d;
    e = v_e;
}

class Sample {
public:
    int sample_id=-1;
    PlayerID_e carried_by=kNone;
//    int rank;
//    string expertise_gain;
    int health{};
    Vitamin vitamin;
//    int cost_a{};
//    int cost_b{};
//    int cost_c{};
//    int cost_d{};
//    int cost_e{};

    double getCostBenefitRatio() const;
    bool isValid() const;
};

double Sample::getCostBenefitRatio() const {
    return (double)health / vitamin.summary();
}

bool Sample::isValid() const {
    return sample_id != -1;
}

// sort samples in descending order
bool sortSamples(const Sample& sample1, const Sample& sample2) {
    return sample1.getCostBenefitRatio() > sample2.getCostBenefitRatio();
}


class Robot {
//    PlayerID_e owned_by;
    string target;
//    int eta;
    int score;
public:
    Vitamin carried_vitamins;
//    int expertise_e;
//    int expertise_d;
//    int expertise_c;
//    int expertise_b;
//    int expertise_a;
    Sample *cur_sample = nullptr; //Sample();
    Robot(string target, int score, const Vitamin& vitamin):
            target{std::move(target)}, score{score}, carried_vitamins{vitamin} {}


    void update(Robot* copy_robot);
    string getTarget() { return target; }
    void setTarget(string new_target){ target = std::move(new_target); };
    string neededMolecules();
    bool isCarriedSample() const;
};

void Robot::update(Robot* copy_robot) {
    target = copy_robot->getTarget();
    score = copy_robot->score;
    carried_vitamins = copy_robot->carried_vitamins;
}

string Robot::neededMolecules() {
    return carried_vitamins.getNeeded(cur_sample->vitamin);
}

bool Robot::isCarriedSample() const {
    return cur_sample != nullptr;
}


class Game {
private:
    bool is_init = true;
    vector<Sample> useful_medicines;
    vector<string> next_moves;
    Robot *me, *enemy;

    Robot* inputPlayer();
public:
    void inputTurn();
    string getCurrentTurn();
};

Robot* Game::inputPlayer() {
    string target;
    int eta;
    int score;
    int storage_a;
    int storage_b;
    int storage_c;
    int storage_d;
    int storage_e;
    int expertise_a;
    int expertise_b;
    int expertise_c;
    int expertise_d;
    int expertise_e;
    cin >> target >> eta >> score >> storage_a >> storage_b >> storage_c >> storage_d >> storage_e >> expertise_a >> expertise_b >> expertise_c >> expertise_d >> expertise_e; cin.ignore();
    Vitamin vitamin {storage_a, storage_b, storage_c, storage_d, storage_e};
    return new Robot{target, score, vitamin};
}

void Game::inputTurn() {
    if (is_init) {
        me = inputPlayer();
    } else {
        // Hollow input
        me->update(inputPlayer());
    }
    enemy = inputPlayer();
    useful_medicines.clear();
    // Ignore
    int available_a;
    int available_b;
    int available_c;
    int available_d;
    int available_e;
    cin >> available_a >> available_b >> available_c >> available_d >> available_e; cin.ignore();
    int sample_count;
    cin >> sample_count; cin.ignore();
    for (int i = 0; i < sample_count; i++) {
        int sample_id;
        int carried_by;
        int rank;
        string expertise_gain;
        int health;
        int cost_a;
        int cost_b;
        int cost_c;
        int cost_d;
        int cost_e;
        cin >> sample_id >> carried_by >> rank >> expertise_gain >> health >> cost_a >> cost_b >> cost_c >> cost_d >> cost_e; cin.ignore();
        Vitamin vitamin {cost_a, cost_b, cost_c, cost_d, cost_e};
        Sample sample{sample_id, (PlayerID_e)carried_by, health,vitamin};
        if (carried_by == PlayerID_e::kNone) {
            useful_medicines.push_back(sample);
        }

    }
    sort(useful_medicines.begin(), useful_medicines.end(), sortSamples);
    is_init = false;
}

string Game::getCurrentTurn() {
    string result;
    if (! next_moves.empty()) {
        // Still got the moves
        result = next_moves.front();
        next_moves.erase(next_moves.begin());
        return result;
    }

    if (! me->isCarriedSample()) {
        if (me->getTarget() == "DIAGNOSIS") {
            me->cur_sample =  new Sample{useful_medicines[0]};
            return "CONNECT " + to_string(useful_medicines[0].sample_id);
        } else {
            me->setTarget("DIAGNOSIS");
            return "GOTO DIAGNOSIS";
        }
    }
    // So, we have a sample and need take needed molecules or send it to the laboratory
    string needed_molecules = me->neededMolecules();
    if (needed_molecules.empty()) {
        next_moves.push_back("CONNECT " + to_string(me->cur_sample->sample_id));
        me->cur_sample = nullptr;
        return "GOTO LABORATORY";
    }
    if (me->getTarget() == "MOLECULES") {
        return "CONNECT " + needed_molecules;
    } else {
        return "GOTO MOLECULES";
    }
}


int main()
{
    // Ignore
    int project_count;
    cin >> project_count; cin.ignore();
    for (int i = 0; i < project_count; i++) {
        int a;
        int b;
        int c;
        int d;
        int e;
        cin >> a >> b >> c >> d >> e; cin.ignore();
    }

    Game game;
    // game loop
    while (1) {
        game.inputTurn();

        cout << game.getCurrentTurn() << endl;
    }
}