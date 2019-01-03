#include <stdio.h>
#include <stdint.h>

#include "common.h"
#include "core.h"
#include "helpers.h"

//standard start start
state_t state = {{{ ROOK,  KNIGHT,  BISHOP,  QUEEN,  KING,  BISHOP,  KNIGHT,  ROOK},
                  { PAWN,    PAWN,    PAWN,   PAWN,  PAWN,    PAWN,    PAWN,  PAWN},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {-PAWN,   -PAWN,   -PAWN,  -PAWN, -PAWN,   -PAWN,   -PAWN, -PAWN},
                  {-ROOK, -KNIGHT, -BISHOP, -QUEEN, -KING, -BISHOP, -KNIGHT, -ROOK}}, 0};
/*
//complicated state to test generate_moves()
state_t state = {{{ ROOK,       0,       0,      0,  KING,  BISHOP,  KNIGHT,  ROOK},
                  {    0,    PAWN,       0, BISHOP,  PAWN,    PAWN,       0,  PAWN},
                  {    0,       0,  KNIGHT,      0,     0,  -BISHOP,       0,     0},
                  { PAWN, -BISHOP,    PAWN,      0,  PAWN,       0,    PAWN,     0},
                  {-PAWN,       0,       0,      0,     0,  BISHOP,   -PAWN,     0},
                  {    0,       0,   -PAWN,      0, -PAWN,       0,       0,     0},
                  {    0,       0,       0,  -PAWN,     0,   -PAWN,       0, -PAWN},
                  {-ROOK, -KNIGHT, -BISHOP,      0, -KING,       0, -KNIGHT, -ROOK}}, 0};
*/
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

    /* who needs recursion eh? 5-nested for-loops, no problem
       ^ this was actually just for debugging a STUPID bug; will delete after next commit,
         but may as well save this for future reference
    //tests generate_moves() function and inverse_move()
    move_t moves1[MAX_NUM_MOVES];
    move_t moves2[MAX_NUM_MOVES];
    move_t moves3[MAX_NUM_MOVES];
    move_t moves4[MAX_NUM_MOVES];
    move_t moves5[MAX_NUM_MOVES];
    //move_t *moves = malloc(sizeof(moves_t) * MAX_NUM_MOVES); //equivalent to above line
    printf("possible moves for white:\n");
    uint8_t num_moves1 = generate_moves(&state, BLACK, moves1);
    for (uint8_t i = 0; i < num_moves1; i++){
        //print_move(&moves1[i]);
        make_move(&state, moves1+i); //nifty pointer trick :)
        uint8_t num_moves2 = generate_moves(&state, WHITE, moves2);
        for (uint8_t j = 0; j < num_moves2; j++){
            make_move(&state, moves2+j);
            uint8_t num_moves3 = generate_moves(&state, BLACK, moves3);
            for (uint8_t k = 0; k < num_moves3; k++){
                make_move(&state, moves3+k);
                uint8_t num_moves4 = generate_moves(&state, WHITE, moves4);
                for (uint8_t l = 0; l < num_moves4; l++){
                    make_move(&state, moves4+l);
                    uint8_t num_moves5 = generate_moves(&state, BLACK, moves5);
                    for (uint8_t m = 0; m < num_moves5; m++){
                        make_move(&state, moves5+m);
                        inverse_move(&state, moves5+m);
                    }
                    inverse_move(&state, moves4+l);
                }
                inverse_move(&state, moves3+k);
            }
            inverse_move(&state, &moves2[j]);
        }
        //print_state(&state);
        inverse_move(&state, &moves1[i]);
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
    int16_t end_score = negamax(&state, &best_move, WHITE, 5, -INFINITY, INFINITY);
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
