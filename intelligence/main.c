#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "core.h"
#include "helpers.h"

//see git commit history for some other pre-typed-out states
//standard start start
state_t state = {{{ ROOK,  KNIGHT,  BISHOP,  QUEEN,  KING,  BISHOP,  KNIGHT,  ROOK},
                  { PAWN,    PAWN,    PAWN,   PAWN,  PAWN,    PAWN,    PAWN,  PAWN},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {-PAWN,   -PAWN,   -PAWN,  -PAWN, -PAWN,   -PAWN,   -PAWN, -PAWN},
                  {-ROOK, -KNIGHT, -BISHOP, -QUEEN, -KING, -BISHOP, -KNIGHT, -ROOK}}};
/*
state_t state = {{{    0,       0,       0,      0,     0,  KING,       0,     0},
                  {    0,       0,       0,      0,     0,       0,       0,    0},
                  {    0,       0,       0,      0,     0,       0,       0,    0},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {    0,       0,       0,      0,     0,       0,       0,     0},
                  {    0,       0,    PAWN,      0,     0,       0,       0,     0},
                  //{    0,       0,       0,      0, -KING,       0,       0,     0}}};
                  //{    0, KNIGHT, -BISHOP, -QUEEN, -KING, -BISHOP, -KNIGHT, -ROOK}}};
                  {    0,       0,       0,      0, -KING,       0,       0,     0}}};
                  */

void main(int argc, char** argv){
    /* Mode of operation:
     *  Make-one-move mode is entered if there are any command line arguments.
     *  Otherwise, a terminal-bsaed game is begun.
     *
     * The format for the command line arguments is decribed below in the
     * "cmd-line make-one-move" section. In this mode, a board state (and other info)
     * is parsed, and the program writes to stdout the best move it can come up with.
     *
     * In the terminal-based game mode, ingo (e.g. the current state of the board) and
     * prompts are written to stdout, allowing one to play a game in the terminal.
     *
     * Finally, there are some commented out debugging calls left at the bottom for
     * when something isn't working right (e.g. castling at the moment [removed completely
     * now]) and specific things need testing.
     */

    if (argc == 4){

        ///////////////////////CMD LINE MAKE-ONE-MOVE///////////////////////////////////////////

        //begin code for cmd line make-one-move mode
        //protocol is to pass in three arguments:
        //  - side: "white" or "black"
        //  - time limit in seconds, represented as one charater code
        //  - board state: a 64 character string of piece chars (upper is white, lower is black)
        // returns best move in format fromrow,fromcol,torow,tocol
        static char piece_chars[7] = {' ','p','r','n','b','q','k'};
        int8_t side = !strcmp(argv[1], "white") ? WHITE : BLACK;
        uint8_t time_limit_s = argv[2][0];
        for (uint8_t i = 0; i < 64; i++){
            for (uint8_t j = 0; j < 7; j++){
                if ((IS_LOWER(argv[3][i]) ? argv[3][i] : CHANGE_CASE(argv[3][i])) == piece_chars[j]){
                    state.pieces[i/8][i%8] = j * (IS_LOWER(argv[3][i]) ? -1 : 1);
                }
            }
        }
        //printf("side: %d, time_limit: %d\n", side, time_limit);
        //print_state(&state);
        move_t best_move = deepening_search(&state, side, time_limit_s);
        printf("%d,%d,%d,%d\n", \
        best_move.from[0], best_move.from[1], best_move.to[0], best_move.to[1]);

    } else {

        ////////////////////////TERMINAL-BASED GAME/////////////////////////////////////////////

        //begin code for terminal-based chess game...

        move_t valid_user_moves[MAX_NUM_MOVES];
        uint8_t num_valid_user_moves;
        uint8_t users_move_is_valid = 0;

        //TODO: add ability to choose to play as black...
        //#define PLAYER_SIDE WHITE
        //if (PLAYER_SIDE == BLACK){
        //    ...

        print_state(&state);
        printf("evaluation for white is: %d\n", evaluate(&state));
        while (1){
            num_valid_user_moves = generate_moves(&state, WHITE, valid_user_moves);
            printf("what move? format fr,fc,tr,tc\n");
            uint8_t fr, fc, tr, tc;
            scanf("%hhu,%hhu,%hhu,%hhu", &fr, &fc, &tr, &tc);
            move_t user_move = get_user_move_instance(&state, fr, fc, tr, tc);
            users_move_is_valid = 0;
            for (uint8_t i = 0; i < num_valid_user_moves; i++){
                if (valid_user_moves[i].from[0] == fr && valid_user_moves[i].from[1] == fc && \
                    valid_user_moves[i].to[0] == tr && valid_user_moves[i].to[1] == tc){
                    users_move_is_valid = 1;
                    break;
                }
            }
            if (!users_move_is_valid){
                printf("that move is invalid!\n");
                continue;
            }
            /*
            move_t user_move = deepening_search(&state, WHITE, 1);
            //memcpy(&user_move, valid_user_moves, sizeof(move_t));
            */
            printf("your move is valid\n");
            make_move(&state, &user_move);
            if (is_checkmated(&state,BLACK)){
                printf("checkmate\n");
                return;
            }
            print_move(&user_move);
            print_state(&state);
            printf("evaluation for white is: %d\n", evaluate(&state));
            move_t comp_move = deepening_search(&state, BLACK, 1);
            //print_negamax_route(&state, &comp_move, BLACK, 3);
            make_move(&state, &comp_move);
            printf("computer plays:\n");
            print_move(&comp_move);
            print_state(&state);
            printf("evaluation for white is: %d\n", evaluate(&state));
            if (is_checkmated(&state,WHITE)){
                printf("checkmate\n");
                return;
            }
        }
    }

    //////////////////////////////DEBUGGING CALLS///////////////////////////////////////
    //begin debugging snippets...

    //printf("starting with state:\n");
    //print_state(&state);

    /*
    //note: these moves aren't all valid, they test the make_move() and inverse_move functions
    //move_t move = {{1,4},{3,4},0,0,0,0}; //white king pawn forward by 2
    move_t move = {{5,5},{0,0},ROOK,0,0,0};
    print_move(&move);
    make_move(&state, &move);
    print_state(&state);
    inverse_move(&state, &move);
    print_state(&state);
    */

    //printf("is white in check? %s\n", in_check(&state, WHITE) ? "yes" : "no");
    //printf("is black in check? %s\n", in_check(&state, BLACK) ? "yes" : "no");

    /*
    //tests generate_moves() function and inverse_move()
    move_t moves[MAX_NUM_MOVES];
    printf("possible moves for white:\n");
    uint8_t num_moves = generate_moves(&state, BLACK, moves);
    for (uint8_t i = 0; i < num_moves; i++){
        print_move(&moves[i]);
        make_move(&state, moves+i); //nifty pointer trick :)
        print_state(&state);
        inverse_move(&state, &moves[i]);
    }
    print_state(&state);
    */

    //printf("white is checkmated? %s\n", is_checkmated(&state, WHITE) ? "yes" : "no");
    //printf("black is checkmated? %s\n", is_checkmated(&state, BLACK) ? "yes" : "no");

    //printf("evaluation for white: %d\n",  evaluate(&state));
    //printf("evaluation for black: %d\n", -evaluate(&state));

    /*
    move_t best_move;
    int16_t end_score = negamax(&state, &best_move, BLACK, 2, -INFINITY, INFINITY);
    printf("best move for black:\n");
    print_move(&best_move);
    printf("yielding an eventual evaluation of: %d\n", end_score);
    print_state(&state);
    */

    //print_negamax_route(&state, NULL, BLACK, 5);

    //move_t best_move = deepening_search(&state, WHITE, 2);
    //print_move(&best_move);
}
