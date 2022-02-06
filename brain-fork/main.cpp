#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "game_simple.h"
#include "game_genetic.h"

using namespace std;

inline bool isEqual(const string& inp, int start, int count) {
    for (int i=start; i<start+count; i++) {
        if (inp[i] != inp[i+count])
            return false;
    }
    return true;
}

string minimizeResult(string prev_result) {
    string result;
    bool changed = false;
    int saved = 0;
    for (int size=(int) (prev_result.size() / 2); size >= 3; size--) {
        int i=0;
        for (; i<prev_result.size()-2*size+1; i++) {
            if (isEqual(prev_result, i, size)) {
                result += '[';
                for (int k=0; k<size; k++)
                    result += prev_result[i+k];
                result += ']';
                i += size*2-1;
                saved += size-2;
                changed = true;
            } else {
                result += prev_result[i];
            }
        }
        for (; i<prev_result.size(); i++) {
            result += prev_result[i];
        }
        if (changed) {
            saved = 0;
            changed = false;
            prev_result = result;
        }
        result = "";
    }
    return prev_result;
}

void test() {
    cout << minimizeResult("A") << endl;
    cout << minimizeResult("AAAAAAA") << endl;
    cout << minimizeResult("AAABBB") << endl;
    cout << minimizeResult("CAAABBBAAABBBCAAABBBAAABBBAAABBBAAABBB") << endl;

    // Imagine: {0, 0, 3} - state, 0 - position,
    rune_state runes[] = {0, 0, 3, 3, 4, 5, 6, 6,0, 0, 3, 3, 4, 5, 6, 6,0, 0, 3, 3, 4, 5, 6, 6,0, 0, 3, 3, 4, 5, 6};
    rune_state spell[] = {5, 1, 1};
    Gene gene = Gene(0, 3, runes);
    cout << "Gene score is: " << gene.getSize(spell) << endl;
    cout << "Gene letter's position is: " << endl;
    for (int i=0; i<3; i++) {
        cout << " " << (int)gene.letter_pos[i];
    }
    cout << endl;
    cout << "Gene string: " << gene.getInfo(spell);
}

int main()
{
    // test string: AMLFDWVYUS MDW JKANHDAN DWQJKHGSQAGHCDQWILUDAJBCSAJL
//    test();
//    return 0;
    srand(0);
    GameSimple game;
    GameSimple new_game;
    string magic_phrase;
    getline(cin, magic_phrase);
    cerr << "Secret message is: " << magic_phrase << endl;

    if (magic_phrase.size() < 50) {
        cout << game.findWays(magic_phrase) << endl;
        return 0;
    }
    // genetic algorithm give bad results when used in plain runes (by-default all contains <space> or, in other notation, 0)
    // So It would be great to little mess-up runes and process first letters of the spell with standard algo
    string quick_start = game.findWays(magic_phrase.substr(0, 50));


    GameGenetic game_gen = GameGenetic(magic_phrase.substr(50), quick_start);
    string genetic = game_gen.findWays();
    string simple = new_game.findWays(magic_phrase);

    cerr << genetic.size() << " " << simple.size() << endl;
    cerr << "Difference between genetic and standard is: " << (int)(genetic.size()-simple.size()) << endl;
    if (simple.size() < genetic.size()) {
        cout << simple << endl;
    } else {
        cout << genetic << endl;
    }
    cout << genetic << endl;
}