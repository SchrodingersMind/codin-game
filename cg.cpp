#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

typedef unsigned long long ull;
typedef unsigned long ul;

ull getEarning(ul* group, int group_size, ul seats, int& iterator) {
//    int group_size = group.size();
    int start_iterator = (iterator == 0) ? group_size-1 : iterator-1;
    ull earns=0;
    if (group_size == 1)
        return group[0];

    while (start_iterator!=iterator && seats>earns) {
        iterator++;
        iterator = iterator%group_size;
        earns += group[iterator];
    }
    if (earns > seats) {
        earns -= group[iterator];
        iterator--;
    } else if (earns + group[(iterator+1)%group_size] < seats) {
        iterator++;
        earns += group[iterator%group_size];
    }
    return earns;
}
int main()
{
    ull earned=0;
    ul seats;
    ul c;
    int group_size;
    cin >> seats >> c >> group_size; cin.ignore();
    ul *groups_queue = new ul[group_size];
    ul *race_earns = new ul[group_size];
    int *iterator_shift = new int[group_size];
    for (int i = 0; i < group_size; i++) {
        int pi;
        cin >> pi; cin.ignore();
        groups_queue[i] = pi;
        race_earns[i] = 0;
        iterator_shift[i] = 0;
    }

    int iterator = group_size - 1;
    if (group_size == 1) {
//        earns += group[0];
        cout << groups_queue[0]*c << endl;
        return 0;
    }

    for (ul i=0; i<c; i++) {
        ul tmp_earned=0;
//        tmp_earned += getEarning(groups_queue, group_size, seats, iterator);
        int start_iterator = iterator; //(iterator == 0) ? group_size-1 : iterator-1;
//        ull tmp_earned=0;

        if (race_earns[start_iterator]) {
            earned += race_earns[start_iterator];
            iterator = iterator_shift[start_iterator];
            continue;
        }

        do {
            iterator++;
            iterator = iterator%group_size;
            tmp_earned += groups_queue[iterator];
        } while (start_iterator!=iterator && seats > tmp_earned);
        if (tmp_earned > seats) {
            tmp_earned -= groups_queue[iterator];
            iterator--;
            if (iterator < 0) {
                iterator = group_size-1;
                cerr << "Wrong output hereeee!" << endl;
            }
            // } else if (tmp_earned + groups_queue[(iterator + 1) % group_size] < seats) {
            //     iterator++;
            //     tmp_earned += groups_queue[iterator % group_size];
        }
        race_earns[start_iterator] = tmp_earned;
        iterator_shift[start_iterator] = iterator;
        earned += tmp_earned;
    }

    cout << earned << endl;
}