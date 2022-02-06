//
// Created by robotknik on 05.02.22.
//

#ifndef BRAINFORK_GAME_GENETIC_H
#define BRAINFORK_GAME_GENETIC_H

#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>

#define CHUNK_SIZE 5
#define RUNES_COUNT 30
#define RUNES_VARIABLES 27
#define GENES_COUNT 30

// from 0 to the RUNES_COUNT
typedef char position;
// from 0 to the RUNES_VARIABLES
typedef char rune_state;

using namespace std;

enum Moves {
    kNone,
    kMoveRight,
    kMoveLeft,
    kRollUp,
    kRollDown,
    kCast
};

struct OneMove {
    Moves move=kNone;
    int count=1;

//    explicit OneMove(Moves move): move(move) {};
};

class GameState {
public:
    position cur_pos=0;
    rune_state runes[RUNES_COUNT];

    GameState();
    GameState(const position start_pos, const rune_state* start_runes): cur_pos{start_pos} {copy_n(start_runes, RUNES_COUNT, runes);}

    void implement(const string& inp);
    // Return number moves that needed to change that rune (without casting it)
    int setRuneState(position rune, rune_state new_state);
};

class Gene {
public:
    position starting_pos{};
    int chunk_size{};
//    rune_state chunk_word[CHUNK_SIZE];
    rune_state runes[RUNES_COUNT];
    position letter_pos[CHUNK_SIZE];

    Gene(position start, int chunk_size_a, rune_state* runes_a);
    Gene();
    void update(int letter_in_word, position new_position);
    void mutate(int infected);
    int getSize(rune_state* chunk_word) const;
    string getInfo(const rune_state* chunk_word);
};

class GameGenetic {
private:
    int inp_pos = 0;
    GameState cur_state;
    rune_state * magic_word;
    string begin;
    int word_size=0;
    Gene genes[GENES_COUNT];

    // Get next chunk from given word
    int getChunk(char *word_chunk);
    // Convert character (like 'A') to comparable number (like 1)
    static char getNumber(char ch);
    // Run microbial GA
    void playRound(rune_state* word_chunk);
public:
    explicit GameGenetic(string  inp_str, string quick_start) {
        word_size = inp_str.size();
        magic_word = new rune_state[word_size];
        for (int i=0; i<word_size; i++) {
            magic_word[i] = getNumber(inp_str[i]);
        }
        begin = std::move(quick_start);
        cur_state.implement(begin);
    };
    ~GameGenetic() {
        delete[] magic_word;
    }

    string findWays();
};


#endif //BRAINFORK_GAME_GENETIC_H
