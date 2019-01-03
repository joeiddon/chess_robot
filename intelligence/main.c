#include <stdio.h>
#include <stdint.h>

#include "common.h"
#include "core.h"
#include "helpers.h"


//standard start start
/*
state_t state = {{{ ROOK,  KNIGHT,  BISHOP,  QUEEN,  KING,  BISHOP,  KNIGHT,  ROOK},
                  { PAWN,    PAWN,    PAWN,   PAWN,  PAWN,    PAWN,    PAWN,  PAWN},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {-PAWN,   -PAWN,   -PAWN,  -PAWN, -PAWN,   -PAWN,   -PAWN, -PAWN},
                  {-ROOK, -KNIGHT, -BISHOP, -QUEEN, -KING, -BISHOP, -KNIGHT, -ROOK}}, 0};
*/

//complicated state to test analyse generate_moves() and other things
state_t state = {{{ ROOK,       0,       0,      0,  KING,  BISHOP,  KNIGHT,  ROOK},
                  {    0,    PAWN,       0, BISHOP,  PAWN,    PAWN,       0,  PAWN},
                  {    0,       0,  KNIGHT,      0,     0,  -BISHOP,       0,     0},
                  { PAWN, -BISHOP,    PAWN,      0,  PAWN,       0,    PAWN,     0},
                  {-PAWN,       0,       0,      0,     0,  BISHOP,   -PAWN,     0},
                  {    0,       0,   -PAWN,      0, -PAWN,       0,       0,     0},
                  {    0,       0,       0,  -PAWN,     0,   -PAWN,       0, -PAWN},
                  {-ROOK, -KNIGHT, -BISHOP,      0, -KING,       0, -KNIGHT, -ROOK}}, 0};

/*
//tests castling
state_t state = {{{ ROOK,  0,            0,   0,  KING,  BISHOP,  KNIGHT,  ROOK},
                  { PAWN,    PAWN,    PAWN,   PAWN,  PAWN,    PAWN,    PAWN,  PAWN},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {-PAWN,   -PAWN,   -PAWN,  -PAWN, -PAWN,   -PAWN,   -PAWN, -PAWN},
                  {-ROOK, -KNIGHT, -BISHOP, -QUEEN, -KING, -BISHOP, -KNIGHT, -ROOK}}, 0};
*/

void main(){
    printf("starting with state:\n");
    print_state(&state);

    /*
    //note: these moves aren't all valid, they test the make_move() and inverse_move functions
    //move_t move = {{1,4},{3,4},0,0,0,0}; //white king pawn forward by 2
    //move_t move = {{0,4},{0,6},0,0,QUEENSIDE,1}; //castling test, queen kingside
    //move_t move = {{0,0},{1,0},PAWN,0,0,1}; //castle invalid test, moving rook away
    move_t move = {{5,5},{0,0},ROOK,0,0,0};
    print_move(&move);
    make_move(&state, &move);
    print_state(&state);
    inverse_move(&state, &move);
    print_state(&state);
    */

    /*
    printf("is white in check? %s\n", in_check(&state, WHITE) ? "yes" : "no");
    printf("is black in check? %s\n", in_check(&state, BLACK) ? "yes" : "no");
    */

    /*
    //tests generate_moves() function and inverse_move()
    move_t moves[MAX_NUM_MOVES];
    printf("possible moves for white:\n");
    uint8_t num_moves = generate_moves(&state, WHITE, moves);
    for (uint8_t i = 0; i < num_moves; i++){
        print_move(&moves[i]);
        make_move(&state, moves+i); //nifty pointer trick :)
        print_state(&state);
        inverse_move(&state, &moves[i]);
    }
    print_state(&state);
    */

    /*
    printf("white is checkmated? %s\n", is_checkmated(&state, WHITE) ? "yes" : "no");
    printf("black is checkmated? %s\n", is_checkmated(&state, BLACK) ? "yes" : "no");
    */

    //printf("evaluation for white: %d\n",  evaluate(&state));
    //printf("evaluation for black: %d\n", -evaluate(&state));

    move_t best_move;
    int16_t end_score = negamax(&state, &best_move, WHITE, 6, -INFINITY, INFINITY);
    printf("best move for white:\n");
    print_move(&best_move);
    printf("yielding an eventual evaluation of: %d\n", end_score);
    //print_state(&state);

    //print_negamax_route(&state, NULL, WHITE, 5);

    /*
    while (1){
        printf("what move? format fr,fc,tr,tc\n");
        uint8_t fr, fc, tr, tc;
        scanf("%hhu,%hhu,%hhu,%hhu", &fr, &fc, &tr, &tc);
        move_t my_move = {{fr, fc}, {tr, tc}, 0, 0, 0, 0}; 
        make_move(&state, &my_move);
        print_move(&my_move);
        print_state(&state);
        printf("evaluation for white is: %d\n", evaluate(&state));
        move_t comp_move;
        print_negamax_route(&state, &comp_move, BLACK, 5);
        make_move(&state, &comp_move);
        printf("computer plays:\n");
        print_move(&comp_move);
        print_state(&state);
        printf("evaluation for white is: %d\n", evaluate(&state));
    }
    */
}
