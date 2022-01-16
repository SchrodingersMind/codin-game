#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class Node {
public:
    Node(): name(-1), estimated(0), f_value(0) {};
    Node(int name, int estimated, int f_value): name(name), estimated(estimated), f_value(f_value) {};

    int name;
    int estimated;
    int f_value;
    int real_dist = 0;
    bool fired = false;
    bool in_queue = false;
};

std::vector<std::vector<int>> map;
std::vector<Node> nodes;

Node aStarGetNextNode(std::vector<Node>& nodes_queue) {
    Node min_node = nodes_queue[0];
    int min_position = 0;
    for (int i=1; i<nodes_queue.size(); i++) {
        Node cur_node = nodes_queue[i];
        if ((cur_node.f_value == min_node.f_value && cur_node.name < min_node.name) || min_node.f_value > cur_node.f_value) {
            min_node = cur_node;
            min_position = i;
        }
    }
    nodes_queue.erase(nodes_queue.begin()+min_position);
    return min_node;
}

std::vector<int> getSiblings(int elem) {
    std::vector<int> result;
    for (int i=0; i<map.size(); i++) {
        if (map[elem][i])
            result.push_back(i);
    }
    return result;
}
std::vector<Node> aStarAlg (int start_elem, int end_elem) {
    std::vector<Node> result;
    std::vector<Node> nodes_queue;
    nodes[start_elem].fired = true;
    nodes_queue.push_back(nodes[start_elem]);
    bool done = false;
    while (! done) {
        Node cur_node = aStarGetNextNode(nodes_queue);
        result.push_back(cur_node);
        cur_node.fired = true;
        if (cur_node.name == end_elem) {
            done = true;
            break;
        }
        std::vector<int> siblings = getSiblings(cur_node.name);
        for (int sib_elem : siblings) {
            Node sib_node = nodes[sib_elem];
            if (sib_node.fired)
                continue;

            sib_node.real_dist = cur_node.real_dist + map[cur_node.name][sib_node.name];
            sib_node.f_value = sib_node.estimated + sib_node.real_dist;

            if (sib_node.in_queue) {
                for (Node& tmp_node : nodes_queue) {
                    if (tmp_node.name == sib_node.name && tmp_node.real_dist > sib_node.real_dist) {
                        tmp_node.real_dist = sib_node.real_dist;
                        tmp_node.f_value = sib_node.f_value;
                        nodes[sib_elem] = sib_node;
                    }
                }
            } else {
                sib_node.in_queue = true;
                nodes[sib_elem] = sib_node;
                nodes_queue.push_back(sib_node);
            }
        }
    }
    return result;
}

int main()
{
    int n;
    int e;
    int start;
    int goal;
    cin >> n >> e >> start >> goal; cin.ignore();
//    nodes.resize(n);
    map.resize(n);

    for (int i = 0; i < n; i++) {
        int heuristic_to_end;
        cin >> heuristic_to_end; cin.ignore();
        map[i].resize(n, 0);
        nodes.emplace_back(i, heuristic_to_end, heuristic_to_end);
    }
//    for (Node node : nodes) {
//        cout << node.name << " " << node.f_value << endl;
//    }

    for (int i = 0; i < e; i++) {
        int x;
        int y;
        int c;
        cin >> x >> y >> c; cin.ignore();
        map[x][y] = c;
        map[y][x] = c;
    }
    std::vector<Node> a_star_result = aStarAlg(start, goal);

    for (Node node : a_star_result) {
        cout << node.name << " " << node.f_value << endl;
    }
//    cout << "0 0" << endl;
}