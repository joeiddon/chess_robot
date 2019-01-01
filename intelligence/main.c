#include <stdio.h>
#include <stdint.h>

#include "common.h"
#include "core.h"
#include "helpers.h"

state_t state = {{{ ROOK,  KNIGHT,  BISHOP,  QUEEN,  KING,  BISHOP,  KNIGHT,  ROOK},
                  { PAWN,    PAWN,    PAWN,   PAWN,  PAWN,    PAWN,    PAWN,  PAWN},
                  {-PAWN,       0,       0,      0,     0,       0,       0,     0},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {-PAWN,   -PAWN,   -PAWN,  -PAWN, -PAWN,   -PAWN,   -PAWN, -PAWN},
                  {-ROOK, -KNIGHT, -BISHOP, -QUEEN, -KING, -BISHOP, -KNIGHT, -ROOK}}, 0};

void main(){
    printf("starting with state:\n");
    print_state(&state);

    /*
    move_t move = {{1,4},{3,4},0,0}; //king pawn forward by 2
    //move_t move = {{0,4},{0,6},0,1}; //castling test
    print_move(&move);
    make_move(&state, &move);
    print_state(&state);
    inverse_move(&state, &move);
    print_state(&state);

    printf("is white in check? %s\n", in_check(&state, WHITE) ? "yes" : "no");
    printf("is black in check? %s\n", in_check(&state, BLACK) ? "yes" : "no");
    */

    move_t moves[MAX_NUM_MOVES];
    //move_t *moves = malloc(sizeof(moves_t) * MAX_NUM_MOVES); //equivalent to above line
    printf("possible moves for white:\n");
    uint8_t num_moves = generate_moves(&state, WHITE, moves);
    for (uint8_t i = 0; i < num_moves; i++){
        print_move(&moves[i]);
        make_move(&state, &moves[i]);
        print_state(&state);
        inverse_move(&state, &moves[i]);
    }

    printf("white is checkmated? %s\n", is_checkmated(&state, WHITE) ? "yes" : "no");
    printf("black is checkmated? %s\n", is_checkmated(&state, BLACK) ? "yes" : "no");

    printf("evaluation for white: %d\n",  evaluate(&state));
    printf("evaluation for black: %d\n", -evaluate(&state));
}
