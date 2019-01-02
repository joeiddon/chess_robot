#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "helpers.h"
#include "core.h"

static char piece_chars[7] = {' ','p','r','n','b','q','k'};
static char *state_invalid_castles_words[4] = {"white kingside","white queenside",\
                                               "black kingside","black queenside"};

void print_state(state_t *state){
    //white pieces are uppper case, black pieces lower case
    printf("------------------\n");
    for (int8_t i = 7; i > -1; i--){
        printf("%d|", i);
        for (int8_t j = 0; j < 8; j++){
            if (state->pieces[i][j] > 0){
                printf("%c|", UPPER(piece_chars[state->pieces[i][j]]));
            } else {
                printf("%c|", piece_chars[-state->pieces[i][j]]);
            }
        }
        printf("\n------------------\n");
    }
    printf(" |0|1|2|3|4|5|6|7|\nallowed castle moves:\n");
    for (uint8_t i = 0; i < 4; i++){
        if (!GET_BIT(state->invalid_castles,i)){
            printf("- %s\n", state_invalid_castles_words[i]);
        }
    }
    printf("------------------\n");
}


void print_move(move_t *move){
    printf("move [%d,%d] => [%d,%d]\n", move->from[0], move->from[1], move->to[0], move->to[1]);
}

void make_move(state_t *state, move_t *move){
    //does not insure move is legal!
    int8_t side = state->pieces[MOVE_FROM] > 0 ? WHITE : BLACK;
    if (move->castle){
        //special case: castling (note: move->from must target the right kind for side)
        //move king...
        state->pieces[BACK_ROW(side)][move->castle==KINGSIDE?6:2] = side*KING;
        state->pieces[BACK_ROW(side)][4] = 0;
        //move castle...
        state->pieces[BACK_ROW(side)][move->castle==KINGSIDE?5:3] = side*ROOK;
        state->pieces[BACK_ROW(side)][move->castle==KINGSIDE?7:0] = 0;
    } else if (move->is_pawn_promotion){
        //special case: pawn promotion (auto to queen :)
        state->pieces[MOVE_TO] = side*QUEEN;
        state->pieces[MOVE_FROM] = 0;
    } else {
        //normal move...
        state->pieces[MOVE_TO] = state->pieces[MOVE_FROM];
        state->pieces[MOVE_FROM] = 0;
    }
    if (move->makes_castle_invalid){
        if (move->from[1]==7 || move->from[1]==4)
        SET_BIT_HIGH(state->invalid_castles,KINGSIDE_BIT(side));
        if (move->from[1]==0 || move->from[1]==4)
        SET_BIT_HIGH(state->invalid_castles,QUEENSIDE_BIT(side));
    }
}

void inverse_move(state_t *state, move_t *move){
    //does the move from to->from
    int8_t side = state->pieces[MOVE_TO] > 0 ? WHITE : BLACK;
    if (move->castle){
        //special case: castling
        //move king...
        state->pieces[BACK_ROW(side)][move->castle==KINGSIDE?6:2] = 0;
        state->pieces[BACK_ROW(side)][4] = side*KING;
        //move castle...
        state->pieces[BACK_ROW(side)][move->castle==KINGSIDE?5:3] = 0;
        state->pieces[BACK_ROW(side)][move->castle==KINGSIDE?7:0] = side*ROOK;
    } else if (move->is_pawn_promotion){
        //special case: pawn promotion (auto to queen :)
        state->pieces[MOVE_TO] = move->piece_taken;
        state->pieces[MOVE_FROM] = side*PAWN;
    } else {
        //normal move...
        state->pieces[MOVE_FROM] = state->pieces[MOVE_TO];
        state->pieces[MOVE_TO] = move->piece_taken;
    }
    if (move->makes_castle_invalid){
        if (move->from[1]==7 || move->from[1]==4)
        SET_BIT_LOW(state->invalid_castles,KINGSIDE_BIT(side));
        if (move->from[1]==0 || move->from[1]==4)
        SET_BIT_LOW(state->invalid_castles,QUEENSIDE_BIT(side));
    }
}

void print_negamax_route(state_t *state, int8_t side, uint8_t depth){
    //To see what negamax had in mind, we sequentially make the best move for the given
    //depth and then decrease the depth by 1 and make the best move for the other side
    //which is what the negamax algorithm works out. At each stage, we print the current state
    move_t move_route[depth];
    printf("analysing negamax's evaluation of the following state, for %s\n", side==WHITE?"white":"black");
    print_state(state);
    int16_t end_score = negamax(state, move_route, side, depth);
    printf("best eval for %s found, after searching to depth %d, was: %d\n", side==WHITE?"white":"black", depth, end_score);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("Negamax expects the following moves from now:\n");
    //make and print the first move
    print_move(move_route);
    make_move(state, move_route);
    print_state(state);
    //pointer to where to store the next move
    move_t *move_pointer = move_route+1;
    uint8_t d = depth-1;
    while (d > 0){
        //switch perspectives
        side *= -1;
        negamax(state, move_pointer, side, d--);
        //make the move
        make_move(state, move_pointer);
        print_move(move_pointer);
        print_state(state);
        move_pointer+=1;
    }
    //iterate over the move_route array backwatds, undoing the moves to reset the state
    move_pointer = move_route+depth;
    while (--move_pointer >= move_route)
    inverse_move(state, move_pointer);
}
