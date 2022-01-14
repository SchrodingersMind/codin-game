#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Don't let the machines win. You are humanity's last hope...
 **/

void getNeighbours(const vector<vector<bool>>& map, int start_x, int start_y, int* r_x, int* d_y) {
    *r_x = -1;
    *d_y = -1;
    for (int i=start_y+1; i<map.size(); i++) {
        if (map[i][start_x]){
            *d_y = i;
            break;
        }
    }
    for (int i=start_x+1; i<map[0].size(); i++) {
        if (map[start_y][i]){
            *r_x = i;
            break;
        }
    }
}

int main()
{
    int width; // the number of cells on the X axis
    cin >> width; cin.ignore();
    int height; // the number of cells on the Y axis
    cin >> height; cin.ignore();
    vector<vector<bool>> map(height+1);
    for (int i = 0; i < height; i++) {
        string line;
        getline(cin, line); // width characters, each either 0 or .
        for (int k=0; k<line.size(); k++) {
            if (line[k] == '0') {
                map[i].push_back(true);
                cerr << "true" << endl;
            } else{
                map[i].push_back(false);
                cerr << "false" << endl;
            }
        }
        // Add extra column
        map[i].push_back(false);
    }
    for (int i=0; i<width+1; i++)
        map[height].push_back(false);

    // Write an action using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;

    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            int r_y=y, r_x= x + 1, d_y= y + 1, d_x=x;
            if (map[y][x]) {
                getNeighbours(map, x, y, &r_x, &d_y);
                if (r_x == -1)
                    r_y = -1;
                if (d_y == -1)
                    d_x = -1;
                cout << x << " " << y << " " << r_x << " " << r_y << " " << d_x << " " << d_y << endl;
            }
        }
    }
}