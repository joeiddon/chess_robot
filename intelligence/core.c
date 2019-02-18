#include <stdio.h>
#include <stdint.h>

#include "common.h"
#include "core.h"
#include "helpers.h"

//special piece moves
const int8_t horse_moves[8][2] = {{1,2},{-1,2},{1,-2},{-1,-2},{2,1},{-2,1},{2,-1},{-2,-1}};
const int8_t king_moves[8][2] = {{0,1},{0,-1},{1,0},{-1,0},{1,1},{-1,1},{1,-1},{-1,-1}};

//advantage given to occupation of these in evaluate
const uint8_t centre_squares[4][2] = {{3,3},{3,4},{4,3},{4,4}};

//when to end negamax search? (global to save making copies and passing about)
uint16_t end_time_s;

move_t deepening_search(state_t *state, int8_t side, uint8_t time_limit_s){
    //implements iterative deepening whilst the time taken is less than the time_limit_s (seconds)
    uint16_t cur_time_s = get_time_s();
    end_time_s = cur_time_s + time_limit_s;
    move_t best_move, temp_best_move;
    uint8_t depth = 0;
    while (cur_time_s < end_time_s){
        //printf("depth %d, best move: ", depth);
        //print_move(&best_move);
        //printf("time to search to depth: %d\n", depth);
        //we only update the best move with the temp move if the previously call
        //finished in time and didn't have to abort
        best_move = temp_best_move;
        int16_t s = negamax(state, &temp_best_move, side, ++depth, -INFINITY, INFINITY);
        if (ABS(s) == INFINITY) return temp_best_move;
        cur_time_s = get_time_s();
    }
    //printf("thought to depth: %d\n", depth);
    return best_move;
}

int16_t negamax(state_t *state, move_t *best_move, int8_t side, uint8_t depth, int16_t alpha, int16_t beta){
    //alpha should be initialised to -INFINITY; beta to INFINITY
    //returns best score and sets best_move to the best_move
    #ifdef DEBUG_NEGAMAX
    printf("~~~~~SPAWNED at depth: %d~~~~~\n", depth);
    print_state(state);
    state_t state_backup;
    copy_state(state, &state_backup);
    #endif
    if (depth > 2 && end_time_s <= get_time_s()){
        //printf("abort");
        return 0;
    }
    if (depth == 0) return side*evaluate(state);
    int16_t best_score = -INFINITY;
    move_t moves[MAX_NUM_MOVES];
    uint8_t num_moves = generate_moves(state, side, moves);
    if (num_moves == 0) return INFINITY;
    uint8_t move_order[MAX_NUM_MOVES]; //array of indicies
    order_moves(moves, num_moves, move_order);
    for (uint8_t i = 0; i < num_moves; i++){
        make_move(state, moves+move_order[i]); //pointer addition! - same as &moves[...]
        int16_t score = -negamax(state,
                                 NULL,
                                 -side,
                                 depth-1, //>0?depth-1:(moves[move_order[i]].piece_taken?1:0), //if quiescence, terminate this branch, otherwise by 1
                                 -beta, -alpha);
        inverse_move(state, moves+move_order[i]);
        if (score > best_score){
            best_score = score;
            if (best_move != NULL) *best_move = moves[move_order[i]];
        }
        if (best_score > alpha) alpha = best_score;
        if (alpha >= beta) return alpha;
    }
    return best_score;
}

int16_t order_moves(move_t *moves, uint8_t num_moves, uint8_t *order){
    //populates the order array with indicies of moves in the moves array
    //ordering them with the most interesting moves first
    //sort method: fill "interesting moves" from the front, and
    //"uninteresting moves" from the back then they will meet in the middle
    uint8_t interesting_index = 0; //post-incremented
    uint8_t uninteresting_index = num_moves; //pre-decremented
    for (uint8_t i = 0; i < num_moves; i++){
        //most basic "interesting test" :)
        if (moves[i].piece_taken){
            order[interesting_index++] = i;
        } else {
            order[--uninteresting_index] = i;
        }
    }
}

int16_t evaluate(state_t *state){
    //evaluates the board for white
    //black's score is -1*this
    if (is_checkmated(state, BLACK)) return  INFINITY;
    if (is_checkmated(state, WHITE)) return -INFINITY;
    int16_t score = 0;
    uint8_t num_whites = 0;
    uint8_t num_blacks = 0;
    int8_t white_king_position[2]; //positions are signed as subtracted later
    int8_t black_king_position[2];
    //material
    for (uint8_t i = 0; i < 8; i++){
        for (uint8_t j = 0; j < 8; j++){
            //just count black and white pieces whilst here
            if (state->pieces[i][j] > 0) num_whites++;
            else if (state->pieces[i][j] < 0) num_blacks++;
            //evaluate material
            switch (state->pieces[i][j]){
                case  PAWN:   score += PAWN_VAL;   break;
                case  ROOK:   score += ROOK_VAL;   break;
                case  KNIGHT: score += KNIGHT_VAL; break;
                case  BISHOP: score += BISHOP_VAL; break;
                case  QUEEN:  score += QUEEN_VAL;  break;

                case -PAWN:   score -= PAWN_VAL;   break;
                case -ROOK:   score -= ROOK_VAL;   break;
                case -KNIGHT: score -= KNIGHT_VAL; break;
                case -BISHOP: score -= BISHOP_VAL; break;
                case -QUEEN:  score -= QUEEN_VAL;  break;

                case KING:
                    white_king_position[0] = i;
                    white_king_position[1] = j;
                    break;
                case -KING:
                    black_king_position[0] = i;
                    black_king_position[1] = j;
                    break;
            }
        }
    }
    //if early game
    if (num_whites + num_blacks > EARLY_GAME_PIECE_THRESH){
        //centre squares
        for (uint8_t i = 0; i < 4; i ++)
        score += state->pieces[centre_squares[i][0]][centre_squares[i][1]] > 0 ? CENTRE_POWER : \
                 state->pieces[centre_squares[i][0]][centre_squares[i][1]] < 0 ? -CENTRE_POWER : 0;

        //king positions - better at edges
        score += CENTRE_DIST(white_king_position[0]) * KING_POS_POWER;
        score += CENTRE_DIST(white_king_position[1]) * KING_POS_POWER;
        score -= CENTRE_DIST(black_king_position[0]) * KING_POS_POWER;
        score -= CENTRE_DIST(black_king_position[1]) * KING_POS_POWER;
    } else { //late game
        //king positions - better at centre
        score -= CENTRE_DIST(white_king_position[0]) * KING_POS_POWER;
        score -= CENTRE_DIST(white_king_position[1]) * KING_POS_POWER;
        score += CENTRE_DIST(black_king_position[0]) * KING_POS_POWER;
        score += CENTRE_DIST(black_king_position[1]) * KING_POS_POWER;
    }
    return score;
}

uint8_t is_checkmated(state_t *state, int8_t side){
    if (!in_check(state, side)) return 0;
    return generate_moves(state, side, NULL) == 0;
}

uint8_t in_check(state_t *state, int8_t side){
    //first locate king...
    uint8_t king_row;
    uint8_t king_col;
    for (uint8_t i = 0; i < 8; i++){
        for (uint8_t j = 0; j < 8; j++){
            if (state->pieces[i][j] == side*KING){
                king_row = i;
                king_col = j;
            }
        }
    }
    //now check for attacks... (see the CHECK_SLIDING_PIECE macro as vital for following this!)
    //pawns
    if (king_row + side >= 0 && king_row + side < 8){
        if (king_col > 0 && state->pieces[king_row+side][king_col-1] == -side*PAWN) return 1;
        if (king_col < 7 && state->pieces[king_row+side][king_col+1] == -side*PAWN) return 1;
    }
    //[note sliding pieces done with for-loops but probably slightly more efficient and/or neater with while-loops]
    //rooks and queen's lateral movement
    if (king_col<7) for (int8_t c = king_col+1; c <  8; c++){ CHECK_SLIDING_PIECE(king_row, c, ROOK) }
    if (king_col>0) for (int8_t c = king_col-1; c >= 0; c--){ CHECK_SLIDING_PIECE(king_row, c, ROOK) }
    if (king_row<7) for (int8_t r = king_row+1; r <  8; r++){ CHECK_SLIDING_PIECE(r, king_col, ROOK) }
    if (king_row>0) for (int8_t r = king_row-1; r >= 0; r--){ CHECK_SLIDING_PIECE(r, king_col, ROOK) }
    //bishops and queen's diaganol movement
    for (int8_t d = 1; d <= MIN(7-king_row, 7-king_col); d++){ CHECK_SLIDING_PIECE(king_row+d, king_col+d, BISHOP) }
    for (int8_t d = 1; d <= MIN(7-king_row,   king_col); d++){ CHECK_SLIDING_PIECE(king_row+d, king_col-d, BISHOP) }
    for (int8_t d = 1; d <= MIN(  king_row, 7-king_col); d++){ CHECK_SLIDING_PIECE(king_row-d, king_col+d, BISHOP) }
    for (int8_t d = 1; d <= MIN(  king_row,   king_col); d++){ CHECK_SLIDING_PIECE(king_row-d, king_col-d, BISHOP) }
    //knights
    for (uint8_t i = 0; i < 8; i++){
        if (ON_BOARD(king_row + horse_moves[i][0], king_col + horse_moves[i][1]) && \
            state->pieces[king_row + horse_moves[i][0]][king_col + horse_moves[i][1]] == -side*KNIGHT) return 1;
    }
    //kings
    for (uint8_t i = 0; i < 8; i++){
        if (ON_BOARD(king_row + king_moves[i][0], king_col + king_moves[i][1]) && \
            state->pieces[king_row + king_moves[i][0]][king_col + king_moves[i][1]] == -side*KING) return 1;
    }
    return 0;
}

uint8_t generate_moves(state_t *state, int8_t side, move_t *moves_array){
    //moves_array is a pointer to an array where the moves will be stored
    //returns the number of moves added to the array,
    //if moves_array is NULL, will just return the number of moves
    uint8_t num_moves = 0;
    for (uint8_t r = 0; r < 8; r++){
        for (uint8_t c = 0; c < 8; c++){
            //if piece from desired side...
            if (IS_MINE(r,c))
            switch (ABS(state->pieces[r][c])){
                case PAWN:
                    if (ON_BOARD(r+side,c)&&IS_EMPTY(r+side,c)){
                        CALL_ADD_MOVE(r+side,c,r+side==BACK_ROW(-side));
                        if (IS_PAWN_ROW(r,side)&&IS_EMPTY(r+2*side,c)) //double move
                        CALL_ADD_MOVE(r+2*side,c,0);
                    }
                    if (ON_BOARD(r+side,c+1)&&IS_THEIRS(r+side,c+1)) CALL_ADD_MOVE(r+side,c+1,r+side==BACK_ROW(-side));
                    if (ON_BOARD(r+side,c-1)&&IS_THEIRS(r+side,c-1)) CALL_ADD_MOVE(r+side,c-1,r+side==BACK_ROW(-side));
                    break;
                //sliding pieces use really hacky macros to keep code length down :/
                //see core.h to understand them (esp. MOVES_SLIDING_PIECE :))
                case QUEEN:
                case ROOK:
                    if (c<7) for (int8_t cc = c+1; cc <  8; cc++){ MOVES_SLIDING_PIECE(r, cc) }
                    if (c>0) for (int8_t cc = c-1; cc >= 0; cc--){ MOVES_SLIDING_PIECE(r, cc) }
                    if (r<7) for (int8_t rr = r+1; rr <  8; rr++){ MOVES_SLIDING_PIECE(rr, c) }
                    if (r>0) for (int8_t rr = r-1; rr >= 0; rr--){ MOVES_SLIDING_PIECE(rr, c) }
                    if (ABS(state->pieces[r][c]) == ROOK) break; //fall through if queen...
                case BISHOP:
                    for (int8_t d=1; d<=MIN(7-r,7-c); d++){ MOVES_SLIDING_PIECE(r+d,c+d) };
                    for (int8_t d=1; d<=MIN(7-r,  c); d++){ MOVES_SLIDING_PIECE(r+d,c-d) };
                    for (int8_t d=1; d<=MIN(  r,7-c); d++){ MOVES_SLIDING_PIECE(r-d,c+d) };
                    for (int8_t d=1; d<=MIN(  r,  c); d++){ MOVES_SLIDING_PIECE(r-d,c-d) };
                    break;
                case KNIGHT:
                    for (uint8_t i = 0; i < 8; i++){
                        if (ON_BOARD(r+horse_moves[i][0],c+horse_moves[i][1]) &&
                            side*state->pieces[r+horse_moves[i][0]][c+horse_moves[i][1]] <= 0)
                        CALL_ADD_MOVE(r+horse_moves[i][0],c+horse_moves[i][1],0);
                    } break;
                case KING:
                    for (uint8_t i = 0; i < 8; i++){
                        if (ON_BOARD(r+king_moves[i][0],c+king_moves[i][1]) &&
                            side*state->pieces[r+king_moves[i][0]][c+king_moves[i][1]] <= 0)
                        CALL_ADD_MOVE(r+king_moves[i][0],c+king_moves[i][1],0);
                    }
            }
        }
    }
    return num_moves;
}

void add_move(state_t *state, move_t *moves_array, uint8_t *num_moves, move_t move, uint8_t side){
    //adds the move to the moves_array (if != NULL) and increments num_moves, if legal
    make_move(state, &move);
    if (!in_check(state, side)){
        if (moves_array != NULL)
        moves_array[*num_moves] = move;
        (*num_moves)++;
    }
    inverse_move(state, &move);
}
