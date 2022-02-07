#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <queue>
#include <algorithm>
#include <set>

using namespace std;

// Object, that is a sub-graph of the main graph
// Contains only connected nodes
class Tree {
public:
    vector<int> tree;
    vector<int> to_cut{};

    explicit Tree(vector<int> new_tree): tree(std::move(new_tree)) {};

    friend ostream& operator<< (ostream& stream, Tree some_tree);

    int getLength(int bob_position, vector<vector<int>> map) const;
    // Return number of nodes, that connected to more than one gateway
    int parseConnected(vector<int> connected_gateways);
    bool isAlive() const;
    int getToCut(int bob_position, vector<int> bob_siblings);
};

ostream & operator<< (ostream& stream, Tree some_tree) {
    for (int cur_node : some_tree.tree) {
        stream << cur_node << " ";
    }
    return stream;
}

int Tree::parseConnected(vector<int> connected_gateways) {
    int cost = 0;
    to_cut.clear();
    for (int cur_node : tree) {
        if (connected_gateways[cur_node] > 1) {
            cost++;
            to_cut.push_back(cur_node);
        }
    }
    return cost;
}

int Tree::getLength(int bob_position, vector<vector<int>> map) const {
    if (find(tree.begin(), tree.end(), bob_position) != tree.end()) {
        return 0;
    }
    int length = INT32_MAX;
    queue<int> open_list;
    open_list.push(bob_position);
    vector<int> closed_list(map.size(), -1);
    bool found = false;

    // BFS. Again
    while(!open_list.empty() && !found) {
        int cur_node = open_list.front();
        open_list.pop();
        for (int sibling : map[cur_node]) {
            if (closed_list[sibling] != -1 || sibling == bob_position)
                continue;
            closed_list[sibling] = cur_node;
            open_list.push(sibling);
            if (find(tree.begin(), tree.end(), sibling) != tree.end()) {
                length = -1;
                int tmp_node = sibling;
                while (tmp_node != -1) {
                    tmp_node = closed_list[tmp_node];
                    length++;
                }
                found = true;
                break;
            }
        }
    }
    return length;
}

bool Tree::isAlive() const {
    return !tree.empty();
}

int Tree::getToCut(int bob_position, vector<int> bob_siblings) {
    int result = -1;
//    auto bob_it = find(tree.begin(), tree.end(), bob_position);
    bob_siblings.insert(bob_siblings.begin(), bob_position);
    if (!to_cut.empty()) {
        result = to_cut[0];
        to_cut.erase(to_cut.begin());
    } else {
        for (int cur_node : bob_siblings) {
            auto node_it = find(tree.begin(), tree.end(), cur_node);
            if (node_it != tree.end()) {
                result = cur_node;
                tree.erase(node_it);
                break;
            }
        }
        if (result == -1) {
            result = tree[0];
            tree.erase(tree.begin());
        }
    }
    return result;
}

class Game {
private:
    int bob_position;
    bool is_init = true;
    vector<vector<int>> map;
    vector<int> gateways;
    // Contains number of gateways? that connected to node with given index
    vector<int> to_gateways;
    vector<Tree> trees;

    void initTrees();
    bool isGateway(int node);
    void findNearestNode(int &cut_1, int &cut_2);
    void newFindNearestNode(int &cut_1, int &cut_2);
    static int getBFSLength(vector<int> closed_list, int start_node);
public:
    Game();

    void inputRound();
    string roundResult();
};

void Game::initTrees() {
    // Now create table that help simplify calculation
    set<int> near_gateways;
    for (int i=0; i<map.size(); i++) {
        for (int sibling : map[i]) {
            if (isGateway(sibling)) {
                to_gateways[i]++;
                near_gateways.insert(i);
            }
        }
    }
    cerr << "to_gateways_array: ";
    for (int cur_node: to_gateways) {
         cerr << cur_node << " ";
    }
    cerr << "\n\n";
    // Now create trees
    vector<int> tmp_tree;
    while (!near_gateways.empty()) {
        int cur_node = *near_gateways.begin();
        near_gateways.erase(near_gateways.begin());
        tmp_tree.clear();
        tmp_tree.push_back(cur_node);
        queue<int> open_list;
        open_list.push(cur_node);

        while (!open_list.empty()) {
            int bfs_node = open_list.front();
            open_list.pop();
            for (int sibling: map[bfs_node]) {
                auto near_it = near_gateways.find(sibling);
                if (near_it != near_gateways.end()) {
                    open_list.push(sibling);
                    tmp_tree.push_back(sibling);
                    near_gateways.erase(near_it);
                }
            }
        }
        trees.emplace_back(tmp_tree);
        cerr << "Add new tree: " << trees.back() << endl;
        tmp_tree.clear();
    }
}

bool Game::isGateway(int node) {
    return std::find(gateways.begin(), gateways.end(), node) != gateways.end();
}

int Game::getBFSLength(vector<int> closed_list, int start_node) {
    int length = -1;
    while (start_node != -1) {
        start_node = closed_list[start_node];
        length += 1;
    }
    return length;
}

void Game::findNearestNode(int &cut_1, int &cut_2) {
    vector<int> closed_list(map.size(), -1);
    queue<int> open_list;
    open_list.push(bob_position);
    // There is a need to admit nodes that have more than one gateway-sibling
    // First element in pair - count of gateway-siblings; Second - one of this gateways
    vector<pair<int,int>> to_gateways(map.size(), {0,-1});
    // To ensure that we proceed all gateways of the same length
    int last_length = -1;
    bool enough = false;

    while (!open_list.empty()) {
        int cur_node = open_list.front();
        open_list.pop();
        if (enough && getBFSLength(closed_list, cur_node) >= last_length)
            break;
        cerr << "Proceed node: " << cur_node << endl;
        for (int sibling : map[cur_node]) {
            if (closed_list[sibling] != -1 || sibling == bob_position)
                continue;
            cerr << "\tNew sibling: " << sibling << endl;
            if (!enough) {
                open_list.push(sibling);
            }
            closed_list[sibling] = cur_node;
            if (isGateway(sibling)) {
                cerr << "Before assign..." << endl;
                to_gateways[cur_node].first++;
                to_gateways[cur_node].second = sibling;
                cerr << "After assign..." << endl;
                if (!enough) {
                    enough = true;
                    int tmp_node = sibling;
                    while (tmp_node != -1) {
                        tmp_node = closed_list[tmp_node];
                        last_length += 1;
                    }
                }
            }
        }
    }
    int max_nodes = 0;
    int max_index = 0;
    for (int i=0; i<to_gateways.size(); i++) {
        if (to_gateways[i].first>max_nodes) {
            max_nodes = to_gateways[i].first;
            max_index = i;
        }
    }
    cut_1 = max_index;
    cut_2 = to_gateways[max_index].second;
}

void Game::newFindNearestNode(int &cut_1, int &cut_2) {
    // Now check for the nearest tree of nodes that connected to any gateway
    // The idea behind this, is that when Bobnet is on  this node, we forced to cut the link between this node and gateway
    // But if this tree contains node that connected to two gateways, bobnet could easily sneak to it and win

    // Now find tree that urgently need to cut some links
    int min_diff=INT32_MAX, min_index=0;
    for (int i=0; i<trees.size(); i++) {
        if (!trees[i].isAlive())
            continue;
        int tmp_diff = trees[i].getLength(bob_position, map)- trees[i].parseConnected(to_gateways);
        cerr << "Tree " << i << " has tmp_diff: " << tmp_diff << endl;
        if (tmp_diff < min_diff) {
            min_diff = tmp_diff;
            min_index = i;
        }
    }
    cut_1 = trees[min_index].getToCut(bob_position, map[bob_position]);
    to_gateways[cut_1]--;
    for (int sibling : map[cut_1]) {
        if (isGateway(sibling)) {
            cut_2 = sibling;
            break;
        }
    }
    return;


    // Now check for the nodes that have two or more sibling-gateways
    vector<int> closed_list(map.size(), -1);
    queue<int> open_list;
    open_list.push(bob_position);
    bool done = false;

    while (!open_list.empty() && !done) {
        int cur_node = open_list.front();
        open_list.pop();
        pair<int, int> mb_to_chop{0, 0};
        cerr << "Proceed " << cur_node << endl;

        for (int sibling : map[cur_node]) {
            if (sibling == bob_position)
                continue;

            if (closed_list[sibling] == -1)
                open_list.push(sibling);
            closed_list[sibling] = cur_node;

            if (isGateway(sibling)) {
                cerr << "\tAdd gateway" << endl;
                mb_to_chop.first++;
                mb_to_chop.second = sibling;
            }
        }
        cerr << "\tGateways: " << mb_to_chop.first << endl;
        if (mb_to_chop.first > 1) {
            cerr << "\tLink that contains " << mb_to_chop.first << " siblings-gateways" << endl;
            cut_1 = cur_node;
            cut_2 = mb_to_chop.second;
            done = true;
        }
    }

    cerr << "\n\nAnd now run standard algo\n" << endl;
    if (!done) {
        findNearestNode(cut_1, cut_2);
    }
}

Game::Game() {
    int n; // the total number of nodes in the level, including the gateways
    int l; // the number of links
    int e; // the number of exit gateways
    cin >> n >> l >> e; cin.ignore();
    map.resize(n);
    to_gateways.resize(n, 0);
    for (int i = 0; i < l; i++) {
        int n1; // N1 and N2 defines a link between these nodes
        int n2;
        cin >> n1 >> n2; cin.ignore();
        map[n1].push_back(n2);
        map[n2].push_back(n1);
    }
    for (int i = 0; i < e; i++) {
        int ei; // the index of a gateway node
        cin >> ei; cin.ignore();
        gateways.push_back(ei);
    }

    initTrees();
}

void Game::inputRound() {
    // The index of the node on which the Bobnet agent is positioned this turn
    cin >> bob_position; cin.ignore();
}

string Game::roundResult() {
    int cut_1, cut_2;
    newFindNearestNode(cut_1, cut_2);
    map[cut_1].erase(std::find(map[cut_1].begin(), map[cut_1].end(), cut_2));
    map[cut_2].erase(std::find(map[cut_2].begin(), map[cut_2].end(), cut_1));
    return to_string(cut_1)+" "+ to_string(cut_2);
}

int main()
{

    Game game = Game();
    // game loop
    while (1) {
        game.inputRound();
        // Example: 3 4 are the indices of the nodes you wish to sever the link between
        cout << game.roundResult() << endl;
    }
}