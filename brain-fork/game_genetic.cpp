//
// Created by robotknik on 05.02.22.
//

#include "game_genetic.h"


// Return minimum (by the module count) number of steps from start to end
// Return > 0 if move in right or up
int getDifference(int start, int end, int count) {
    int steps = abs(end-start);
//    position p_min = min(end, start);
//    position p_max = max(end, start);
    if (steps > count/2) {
        steps = count - steps;
        if (start < end) {
            // Move in left side (or down)
            steps = -steps;
        }
    } else if (start > end) {
        steps = -steps;
    }
    return steps;
}

GameState::GameState() {
    cur_pos = 15;
    for (char & rune : runes) {
        rune = 0;
    }
}

void GameState::implement(const string &inp) {
    for (int i=0; i<inp.size(); i++) {
        char ch = inp[i];
        switch(ch) {
            case '>':
                cur_pos += 1;
                cur_pos = (char)(cur_pos%RUNES_COUNT);
                break;
            case '<':
                cur_pos -= 1;
                cur_pos = (char)(cur_pos%RUNES_COUNT);
                break;
            case '+':
                if (runes[cur_pos] == RUNES_VARIABLES-1)
                    runes[cur_pos] = 0;
                else
                    runes[cur_pos] += 1;
                break;
            case '-':
                if (runes[cur_pos] == 0)
                    runes[cur_pos] = RUNES_VARIABLES-1;
                else
                    runes[cur_pos] -= 1;
                break;
            case '.':
                break;
            case '[':
                if (runes[cur_pos] == 0) {
                    while (inp[i] != ']')
                        i++;
                    i++;
                }
                break;
            case ']':
                if (runes[cur_pos != 0]) {
                    while(inp[i] != '[')
                        i--;
                }
                break;
            default:
                cerr << "Unknown character in string " << ch << endl;
        }
    }
}

int GameState::setRuneState(position rune, rune_state new_state) {
    int steps = 0;
    // First, find the best way to move
    int diff = abs(rune-cur_pos);
//    bool is_inner =  < ;
    if (diff < RUNES_COUNT/2) {
        steps = diff;
    } else {
        steps = RUNES_COUNT - diff;
    }
    cur_pos = rune;

    // Now, change rune state
    diff = abs(new_state-runes[rune]);
//    is_inner = abs(new_state-runes[rune]) < RUNES_VARIABLES/2;
    if (diff < RUNES_VARIABLES/2) {
        steps += diff;
    } else {
        steps += RUNES_VARIABLES - diff;
    }
    runes[rune] = new_state;
    return steps;
}




Gene::Gene() {
    starting_pos=0;
    chunk_size=0;
}

Gene::Gene(position start, int chunk_size_a, rune_state* runes_a) {
    starting_pos = start;
    chunk_size = chunk_size_a;
//    chunk_word = chunk_word_a;
    copy_n(runes_a, RUNES_COUNT, runes);
    for (int i=0; i<chunk_size; i++) {
        letter_pos[i] =(char) (rand() % RUNES_COUNT);
    }
}

int Gene::getSize(rune_state* chunk_word) const {
    int result=0;
    GameState test_state = GameState(starting_pos, runes);
    for (int i=0; i<chunk_size; i++) {
        result += test_state.setRuneState(letter_pos[i], chunk_word[i]);
        result += 1;
    }
    return result;
}

void Gene::update(int letter_in_word, position new_position) {
    letter_pos[letter_in_word] = new_position;
}

void Gene::mutate(int infected) {
    int to_mutate;
    while ((to_mutate=rand()%chunk_size) == infected)
        ;
    letter_pos[to_mutate] = (char) (rand()%RUNES_COUNT);

    // More mutation
    while ((to_mutate=rand()%chunk_size) == infected)
        ;
    letter_pos[to_mutate] = (char) (rand()%RUNES_COUNT);
}

string Gene::getInfo(const rune_state* chunk_word) {
    string result;
    position cur_pos = starting_pos;
    int tmp;
    for (int i=0; i<chunk_size; i++) {
        // 1. Get path
        position sym_pos = letter_pos[i];
        tmp = getDifference(cur_pos, sym_pos, RUNES_COUNT);
        if (tmp > 0) {
            result.resize(result.size()+tmp, '>');
        } else {
            result.resize(result.size()-tmp, '<');
        }
        cur_pos = sym_pos;
        // 2. Change letter
        rune_state target_state = chunk_word[i];
        tmp = getDifference(runes[cur_pos], target_state, RUNES_VARIABLES);
        if (tmp > 0) {
            result.resize(result.size()+tmp, '+');
        } else {
            result.resize(result.size()-tmp, '-');
        }
        runes[cur_pos] = target_state;
        // 3. Spell letter
        result += '.';
    }
    return result;
}



char GameGenetic::getNumber(char ch) {
    if (ch == ' ')
        return 0;
    return (char) (ch - 'A' + 1);
}

int GameGenetic::getChunk(rune_state *word_chunk) {
    int i=0;
    for (; i<CHUNK_SIZE && i+inp_pos < word_size; i++) {
        word_chunk[i] = magic_word[i+inp_pos];
    }
    inp_pos += i;
    return i;
}

// In GA this calls Fitness Function
void GameGenetic::playRound(rune_state* word_chunk) {
    int first_comp_int = rand()%GENES_COUNT;
    int second_comp_int;
    Gene& first_comp = genes[first_comp_int];
    while ((second_comp_int=rand()%GENES_COUNT) == first_comp_int)
        ;
    Gene& second_comp = genes[second_comp_int];
    int copied_cell = rand()%first_comp.chunk_size;
    if (first_comp.getSize(word_chunk) < second_comp.getSize(word_chunk)) {
        second_comp.update(copied_cell, first_comp.letter_pos[copied_cell]);
        second_comp.mutate(copied_cell);
    } else {
        first_comp.update(copied_cell, second_comp.letter_pos[copied_cell]);
        first_comp.mutate(copied_cell);
    }
}

string GameGenetic::findWays() {
    string result = begin;
    rune_state word_chunk[CHUNK_SIZE];
    cerr << "\nInitialized result have length: " << result.size() << endl;
    while (inp_pos != word_size) {
        int chunk_size = getChunk(word_chunk);
        // Init general population
        for (auto & gene : genes) {
            gene = Gene(cur_state.cur_pos, chunk_size, cur_state.runes);
        }
        for (int round=0; round<90250; round++) {
            playRound(word_chunk);
        }
        int min_score=genes[0].getSize(word_chunk), min_index=0;
        for (int i=1; i<GENES_COUNT; i++) {
            int tmp_score = genes[i].getSize(word_chunk);
            if (tmp_score < min_score) {
                min_score = tmp_score;
                min_index = i;
            }
        }
        cerr << "Result minimum score is: " << min_score << endl;
        result += genes[min_index].getInfo(word_chunk);
        cerr << "Now result length is: " << result.size() << endl;
    }
    return result;
}