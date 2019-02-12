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
                  { PAWN,       0,    PAWN,      0,     0,       0,       0,     0},
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
     * Or, if a different type is sent, it will return some info.
     *
     * In the terminal-based game mode, ingo (e.g. the current state of the board) and
     * prompts are written to stdout, allowing one to play a game in the terminal.
     *
     * Finally, there are some commented out debugging calls left at the bottom for
     * when something isn't working right (e.g. castling at the moment [removed completely
     * now]) and specific things need testing.
     */

    if (argc > 1){

        ///////////////////////CMD-LINE MAKE-ONE-MOVE///////////////////////////////////////////

        //begin code for cmd line make-one-move mode
        /* ---Interface---
         *
         * The first argument is the board:
         *   - board state: a 64 character string of piece chars (upper is white, lower is black).
         * The second argument indicates the side:
         *   - side: "white" or "black".
         * The third argument is the type of request:
         *   - type: "eval" or "move".
         *
         * If the type is "eval", then no further arguments are required.
         * In this case, three values are returned:
         *      - checkmate status: is "side" checkmated ? 1 or 0;
         *      - evaluation of state: what is returned form evaluate(...);
         *      - available moves: a 3d JSON array of legal moves for "side".
         *
         * If the type is "move", then a further fourth argument is required:
         *   - thinking time limit in seconds: represented as one charater code
         * In this case, a single JSON array is returned representing the
         * best move for "side".
         *
An example "eval" request:
$ ./chess_ai "RNBQKBNRPPPPPPPP                                pppppppprnbqkbnr" white eval
0 0 [[[0,1],[2,2]],[[0,1],[2,0]],[[0,6],[2,7]],[[0,6],[2,5]],[[1,0],[2,0]],[[1,0],[3,0]],[[1,1],[2,1]],[[1,1],[3,1]],[[1,2],[2,2]],[[1,2],[3,2]],[[1,3],[2,3]],[[1,3],[3,3]],[[1,4],[2,4]],[[1,4],[3,4]],[[1,5],[2,5]],[[1,5],[3,5]],[[1,6],[2,6]],[[1,6],[3,6]],[[1,7],[2,7 ]],[[1,7],[3,7]]]
         */
        static char piece_chars[7] = {' ','p','r','n','b','q','k'};
        //process board state string
        for (uint8_t i = 0; i < 64; i++){
            for (uint8_t j = 0; j < 7; j++){
                if ((IS_LOWER(argv[1][i]) ? argv[1][i] : CHANGE_CASE(argv[1][i])) == piece_chars[j]){
                    state.pieces[i/8][i%8] = j * (IS_LOWER(argv[1][i]) ? -1 : 1);
                }
            }
        }
        //process "side" argument
        int8_t side = !strcmp(argv[2], "white") ? WHITE : BLACK;
        //process request type
        if (!strcmp(argv[3], "eval")){ //evaluate
            printf("%s ", is_checkmated(&state, side) ? "1" : "0");
            printf("%d ", evaluate(&state));
            move_t moves[MAX_NUM_MOVES];
            uint8_t num_moves;
            num_moves = generate_moves(&state, side, moves);
            printf("[");
            for (uint8_t i = 0; i < num_moves; i++){
                printf("[[%d,%d],[%d,%d]]", moves[i].from[0],
                                            moves[i].from[1],
                                            moves[i].to[0],
                                            moves[i].to[1]);
                if (i != num_moves-1) printf(",");
            }
            printf("]\n");
        } else { //find best move
            uint8_t time_limit_s = argv[4][0];
            printf("time_limit_s: %d\n", time_limit_s);
            move_t best_move = deepening_search(&state, side, time_limit_s);
            printf("[[%d,%d],[%d,%d]]\n", best_move.from[0],
                                          best_move.from[1],
                                          best_move.to[0],
                                          best_move.to[1]);
        }

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
#define THINK_TIME_S 4
            /*
            move_t user_move = deepening_search(&state, WHITE, THINK_TIME_S);
            //memcpy(&user_move, valid_user_moves, sizeof(move_t));
            */
            printf("your move is valid\n");
            make_move(&state, &user_move);
            print_move(&user_move);
            print_state(&state);
            if (is_checkmated(&state,BLACK)){
                printf("black checkmated\n");
                return;
            }
            printf("evaluation for white is: %d\n", evaluate(&state));
            move_t comp_move = deepening_search(&state, BLACK, THINK_TIME_S);
            //print_negamax_route(&state, &comp_move, BLACK, 3);
            make_move(&state, &comp_move);
            printf("computer plays:\n");
            print_move(&comp_move);
            print_state(&state);
            printf("evaluation for white is: %d\n", evaluate(&state));
            if (is_checkmated(&state,WHITE)){
                printf("white checkmated\n");
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
