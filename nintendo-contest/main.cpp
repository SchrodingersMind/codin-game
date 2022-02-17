#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <bitset>
#include <thread>
#include <future>

#pragma GCC optimize("O3,inline")
#pragma GCC optimize("O3,fast-math,inline")
#pragma GCC optimize("Ofast,inline")
#pragma GCC flag("pthread")

using namespace std;

unsigned int* b;
vector<unsigned int*> decoded_results;

struct PossibleMoves_t {
    int count;

};

void printData(const unsigned int* data, int size) {
    for(int i = 0; i < size / 16; i++) {
        if (i > 0) {
            cout << ' ';
        }
        std::bitset<32> x(data[i]);
        cout << x; // << b[i];       // print result
    }
    cout << endl;
    for(int i = 0; i < size / 16; i++) {
        if (i > 0) {
            cout << ' ';
        }
//        std::bitset<32> x(data[i]);
        cout << setfill('0') << setw(8) << hex << data[i];
    }
    cout << endl;
    cout << endl;
}

inline char getBit(const unsigned int* data, int counter) {
    return (char) ((data[counter / 32] >> (counter % 32)) & 1);
}
inline char getBit(const char* data, int counter) {
    return data[counter];
}

inline void setBit(unsigned int* data, int counter, char value) {
    if (getBit(data, counter) != value) {
        if (value == 0) {
            data[counter / 32] -= 1 << counter;
        } else {
            data[counter / 32] += 1 << counter;
        }
    }
}
inline void setBit(char* data, int counter, char value) {
    data[counter] = value;
}

void encodeValue(int size, const unsigned int* a) {
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            unsigned int first_operation = (a[i / 32] >> (i % 32)) & 1;
            unsigned int second_operation = (a[j / 32 + size / 32] >> (j % 32)) & 1;
            b[(i + j) / 32] ^= ( first_operation & second_operation ) << ((i + j) % 32);
        }

    cout << "Input message:" << endl;
    printData(a, size);
    cout << "Encoded messag:" << endl;
    printData(b, size);

//    for(int i = 0; i < size / 16; i++) {
//        if (i > 0) {
//            cout << ' ';
//        }
//        std::bitset<32> x(b[i]);
//        cout << x; // << b[i];       // print result
//    }
    cout << endl;
}

inline char getXored(unsigned int* possible_ai, unsigned int* possible_aj, int counter, int size, bool use_last) {
    bool is_one=false;
    int counter_top = (use_last) ? counter : counter-1;
    int start = (use_last) ? 0 : 1;
    for (int i=max(start, counter-size+1); i<size && i<counter_top; i++) {
        if (getBit(possible_ai, i) && getBit(possible_aj, counter-i))
            is_one = !is_one;
    }
    return (char)is_one;
}

char getReversedXored(unsigned int* possible_ai, unsigned int* possible_aj, int counter, int size, bool use_last) {
    bool is_one=false;
    int counter_top = (use_last) ? 2*size-1 : 2*size-2;
    int start = (use_last) ? 2*size-1-counter : 2*size-counter;
    for (int i=max(start, counter-size+1); i<size && i<counter_top; i++) {
        if (getBit(possible_ai, i) && getBit(possible_aj, counter-i))
            is_one = !is_one;
    }
    return (char)is_one;
}

vector<pair<char, char>>
getPossibleData(unsigned int *possible_ai, unsigned int *possible_aj, int counter, int size, bool reversed) {
    vector<pair<char, char>> result;
    if (counter == 0) {
        if (getBit(b, 0) == 1) {
            return {pair<char,char>{1, 1}};
        } else {
            return {pair<char,char>{0,0}, pair<char,char>{0,1}, pair<char,char>{1,0}};
        }
    }
    // Calculate current bit before xoring with values that undefined in current iteration
    char cur_bit = getXored(possible_ai, possible_aj, counter, size, false);

    if (cur_bit == getBit(b, counter)) {
        // So, we need to save that flag state
        // In other words, xor it only with zeros (or two one-s)
        result.emplace_back(0, 0);
        if (getBit(possible_ai, 0) == 1) {
            if (getBit(possible_aj, 0) == 1) {
                result.emplace_back(1, 1);
            } else {
                result.emplace_back(0, 1);
            }
        } else if (getBit(possible_aj, 0) == 1) {
            result.emplace_back(1, 0);
        } else {
            result.emplace_back(1, 1);
            result.emplace_back(1, 0);
            result.emplace_back(0, 1);
        }
    } else {
        if(getBit(possible_ai, 0) == 1) {
            result.emplace_back(1, 0);
            if (getBit(possible_aj, 0) == 0) {
                result.emplace_back(1, 1);
            } else {
                result.emplace_back(0, 1);
            }
        } else if (getBit(possible_aj, 0) == 1) {
            result.emplace_back(0, 1);
            result.emplace_back(1, 1);
        }
    }
    return result;
}

void decodeRecursive(unsigned int* possible_ai, unsigned int* possible_aj, int counter, int size) {
    if (counter == size) {
//        if (possible_ai[0] == 0x00000083 || possible_ai[0] == 0x000000e5 || possible_ai[0] == 0x00000001) {
//            cout << "Found value that shouldnt be skipped!!!" <<endl;
//        }
        for (int i=size; i<2*size-1; i++) {
            if (getXored(possible_ai, possible_aj, i, size, true) != getBit(b, i)) {
                return;
            }
        }
        auto* a = new unsigned int[size / 16];
        for (int i=0; i<size/32; i++) {
            a[i] = possible_ai[i];
            a[i + size / 32] = possible_aj[i];
        }
        cout << "Found new value: " <<endl;
        printData(a, size);
        decoded_results.push_back(a);
        return;
    }

    auto possible_moves = getPossibleData(possible_ai, possible_aj, counter, size, false);
    for (auto move : possible_moves) {
        setBit(possible_ai, counter, move.first);
        setBit(possible_aj, counter, move.second);
        decodeRecursive(possible_ai, possible_aj, counter+1, size);
    }
}

inline char getXoredNew(char* possible_ai, char* possible_aj, int counter, int size, bool use_last) {
    bool is_one=false;
    int counter_top = (use_last) ? counter : counter-1;
    int start = (use_last) ? 0 : 1;
    for (int i=max(start, counter-size+1); i<size && i<counter_top; i++) {
        if (getBit(possible_ai, i) && getBit(possible_aj, counter-i))
            is_one = !is_one;
    }
    return (char)is_one;
}

vector<pair<char, char>>
getPossibleDataNew(char *possible_ai, char *possible_aj, int counter, int size, bool reversed) {
    vector<pair<char, char>> result;
    if (counter == 0) {
        if (getBit(b, 0) == 1) {
            return {pair<char,char>{1, 1}};
        } else {
            return {pair<char,char>{0,0}, pair<char,char>{0,1}, pair<char,char>{1,0}};
        }
    }
    // Calculate current bit before xoring with values that undefined in current iteration
    char cur_bit = getXoredNew(possible_ai, possible_aj, counter, size, false);

    if (cur_bit == getBit(b, counter)) {
        // So, we need to save that flag state
        // In other words, xor it only with zeros (or two one-s)
        result.emplace_back(0, 0);
        if (getBit(possible_ai, 0) == 1) {
            if (getBit(possible_aj, 0) == 1) {
                result.emplace_back(1, 1);
            } else {
                result.emplace_back(0, 1);
            }
        } else if (getBit(possible_aj, 0) == 1) {
            result.emplace_back(1, 0);
        } else {
            result.emplace_back(1, 1);
            result.emplace_back(1, 0);
            result.emplace_back(0, 1);
        }
    } else {
        if(getBit(possible_ai, 0) == 1) {
            result.emplace_back(1, 0);
            if (getBit(possible_aj, 0) == 0) {
                result.emplace_back(1, 1);
            } else {
                result.emplace_back(0, 1);
            }
        } else if (getBit(possible_aj, 0) == 1) {
            result.emplace_back(0, 1);
            result.emplace_back(1, 1);
        }
    }
    return result;
}
void decodeRecursiveNew(char* possible_ai, char* possible_aj, int counter, int size) {
    if (counter == size) {
//        if (possible_ai[0] == 0x00000083 || possible_ai[0] == 0x000000e5 || possible_ai[0] == 0x00000001) {
//            cout << "Found value that shouldnt be skipped!!!" <<endl;
//        }
        for (int i=size; i<2*size-1; i++) {
            if (getXoredNew(possible_ai, possible_aj, i, size, true) != getBit(b, i)) {
                return;
            }
        }
        auto* a = new unsigned int[size / 16];
        for (int i=0; i<size; i++) {
            setBit(a, i, getBit(possible_ai, i));
//            a[i] = possible_ai[i];
            setBit(a, i+size, getBit(possible_aj, i));
//            a[i + size / 32] = possible_aj[i];
        }
        cout << "Found new value: " <<endl;
        printData(a, size);
        decoded_results.push_back(a);
        return;
    }

    auto possible_moves = getPossibleDataNew(possible_ai, possible_aj, counter, size, false);
    future<void> tmp[4];
    for (int i=0; i<possible_moves.size(); i++) {
        auto move = possible_moves[i];
        setBit(possible_ai, counter, move.first);
        setBit(possible_aj, counter, move.second);
        if (possible_moves.size() == 4) {
            tmp[i] = std::async(std::launch::async, [&possible_ai, &possible_aj, counter, size] {
                char *new_ai = new char[size];
                char *new_aj = new char[size];
                for (int i = 0; i <= counter; i++) {
                    setBit(new_ai, i, getBit(possible_ai, i));
                    setBit(new_aj, i, getBit(possible_aj, i));
                }
                decodeRecursiveNew(new_ai, new_aj, counter + 1, size);
                delete[] new_ai;
                delete[] new_aj;
            });
//            tmp[i] = future1;
        } else {
            decodeRecursiveNew(possible_ai, possible_aj, counter + 1, size);
        }
    }
}

void decodeValue(int size, unsigned int* a) {
    unsigned int* aj = a+(size/32);
    unsigned int* ai = a;

    char *new_ai = new char[size];
    char *new_aj = new char[size];
    for (int i=0; i<size; i++) {
        setBit(new_ai, i, getBit(ai, i));
        setBit(new_aj, i, getBit(aj, i));
    }

    decodeRecursiveNew(new_ai, new_aj, 0, size);
    cout << "Found possible keys" << endl;
    for (auto result : decoded_results) {
        printData(result, size);
    }
}

void test_func(int size, unsigned int* a) {
    cout << "Test print func: a:" << endl;
    printData(a, size);
    cout << "ai" << endl;
    printData(a, size/2);
    cout << "aj" << endl;
    printData(a+(size/32), size/2);

    unsigned int tmp = a[0];
    cout << "\nTest getBit Func:" << endl;
    cout << "getBit(a, 2) = " << (int)getBit(a, 2) << endl;
    a[0] = 0;
    cout << "getBit(*0, 2) = " << (int)getBit(a, 2) << endl;
    a[0] = 0xffffffff;
    cout << "getBit(*0xffffff, 2) = " << (int)getBit(a, 2) << endl;
    a[0] = tmp;

    cout << "\nTest setBit func:" << endl;
    cout << "setBit(a, 33, 0); a=" << endl;
    setBit(a, 33, 0);
    printData(a, size);

    cout <<  "\nTest getXored:" << endl;
//    a[0] = 0xffffffff;
    cout << "getXored(even) " << (int)getXored(a, a+(size/32), 3, size, false) << endl;
    cout << "getXored(even, use_last) " << (int)getXored(a, a+(size/32), 3, size, true) << endl;
    cout << "getXored(odd) " << (int)getXored(a, a+(size/32), 3, size, false) << endl;


    cout << "\nTest getPossibleData(0):" << endl;
    auto tmp_moves = getPossibleData(a, a + (size / 32), 0, size, false);
    for (auto move : tmp_moves) {
        cout << "Move: " << (int)move.first << " " << (int)move.second <<endl;
    }
    cout << "getPossibleData(2):" <<endl;
    tmp_moves = getPossibleData(a, a + (size / 32), 2, size, false);
    for (auto move : tmp_moves) {
        cout << "Move: " << (int)move.first << " " << (int)move.second <<endl;
    }
}

int main()
{
    int size;
    cin >> size; cin.ignore();
//    string line;
//    getline(cin, line);
    auto* a = new unsigned int[size / 16]; // <- input tab to encrypt
    b = new unsigned int[size / 16]; // <- output tab

    for (int i = 0; i < size / 16; i++) {   // Read size / 16 integers to a
        cin >> hex >> a[i];
    }

    for (int i = 0; i < size / 16; i++) {   // Write size / 16 zeros to b
        b[i] = 0;
    }

    encodeValue(size, a);
//    test_func(size, a);
//    return 0;

    decodeValue(size, a);

    delete[] a;
    delete[] b;
}
