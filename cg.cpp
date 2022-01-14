#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main()
{
    int n;
    cin >> n; cin.ignore();
    vector<int> exchanges_array;
    for (int i = 0; i < n; i++) {
        int v;
        cin >> v; cin.ignore();
        exchanges_array.push_back(v);
    }

    int max_loss = 0;
    int max_point = 0;
    for (int & iter : exchanges_array) {
        if (max_point < iter)
            max_point = iter;
        else if (iter-max_point < max_loss)
            max_loss = iter-max_point;
    }

    // Write an answer using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;

    cout << max_loss << endl;
}