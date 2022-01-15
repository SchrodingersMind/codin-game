#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class Node {
public:
    int elem{};
    int max_path=0;
};

void getAllSiblings(int elem) {

}

int getMaxPath(int elem, const vector<vector<int>>& map, vector<int>& max_paths) {
    int global_max_path = 1;

    for (int sib_elem : map[elem]) {
        int sib_max_path = getMaxPath(sib_elem, map, max_paths);
        if (global_max_path < sib_max_path+1) {
            global_max_path = sib_max_path+1;
        }
    }
    max_paths[elem] = global_max_path;
    return global_max_path;
}

int main()
{
    vector<int*> all_paths;
    int max_value=0;
    int n; // the number of relationships of influence
    cin >> n; cin.ignore();
    for (int i = 0; i < n; i++) {
        int x; // a relationship of influence between two people (x influences y)
        int y;
        cin >> x >> y; cin.ignore();
        all_paths.push_back(new int[2] {x, y});
        if (y > max_value)
            max_value = y;
        if (x > max_value)
            max_value = x;
    }

    std::vector<std::vector<int>> map (max_value+1);
    for (auto pair : all_paths) {
        map[pair[0]].push_back(pair[1]);
    }

    int max_path = 0;
    vector<int> max_lengths(max_value+1);
    int max_length = 1;
    for (int i=0; i < map.size(); i++) {
        int tmp_result = getMaxPath(i, map, max_lengths);
        if (tmp_result > max_length)
            max_length = tmp_result;
    }


    // The number of people involved in the longest succession of influences
    cout << max_length << endl;
}