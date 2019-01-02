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

int16_t negamax(state_t *state, move_t *best_move, int8_t side, uint8_t depth, int16_t alpha, int16_t beta){
    //alpha should be initialised to -INFINITY; beta to INFINITY
    //returns best score and sets best_move to the best_move
    #ifdef DEBUG_NEGAMAX
    printf("~~~~~SPAWNED at depth: %d~~~~~\n", depth);
    print_state(state);
    #endif
    if (depth == 0) return side*evaluate(state);
    int16_t best_score = -INFINITY;
    move_t moves[MAX_NUM_MOVES];
    uint8_t num_moves = generate_moves(state, side, moves);
    for (uint8_t i = 0; i < num_moves; i++){
        make_move(state, moves+i);
        int16_t score = -negamax(state, NULL, -side, depth-1, -beta, -alpha);
        inverse_move(state, moves+i);
        if (score > best_score){
            best_score = score;
            if (best_move != NULL) *best_move = moves[i];
        }
        if (best_score > alpha) alpha = best_score;
        if (alpha >= beta) { //prune tree as we have found a score we can be sure of achieving better than their best score
        /*printf("branching factor: %d\n", i);*/return alpha;}
    }
    return best_score;
}

int16_t evaluate(state_t *state){
    //evaluates the board for white
    //black's score is -1*this
    if (is_checkmated(state, BLACK)) return  INFINITY;
    if (is_checkmated(state, WHITE)) return -INFINITY;
    int16_t score = 0;
    //material
    for (uint8_t i = 0; i < 8; i++){
        for (uint8_t j = 0; j < 8; j++){
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
            }
        }
    }
    //centre squares
    for (uint8_t i = 0; i < 4; i ++)
    score += state->pieces[centre_squares[i][0]][centre_squares[i][1]] > 0 ? CENTRE_POWER : \
             state->pieces[centre_squares[i][0]][centre_squares[i][1]] < 0 ? -CENTRE_POWER : 0;

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
    int8_t cc,rr;
    for (uint8_t r = 0; r < 8; r++){
        for (uint8_t c = 0; c < 8; c++){
            //if piece from desired side...
            if (IS_MINE(r,c))
            switch (ABS(state->pieces[r][c])){
                case PAWN:
                    if (ON_BOARD(r+side,c)&&IS_EMPTY(r+side,c)){
                        CALL_ADD_MOVE(r+side,c,IS_END_ROW(r+side,side),0,0);
                        if(r==(side==WHITE?1:6)&&IS_EMPTY(r+2*side,c)) //double move
                        CALL_ADD_MOVE(r+2*side,c,0,0,0);
                    }
                    if (ON_BOARD(r+side,c+1)&&IS_THEIRS(r+side,c+1)) CALL_ADD_MOVE(r+side,c+1,IS_END_ROW(r+side,side),0,0);
                    if (ON_BOARD(r+side,c-1)&&IS_THEIRS(r+side,c-1)) CALL_ADD_MOVE(r+side,c-1,IS_END_ROW(r+side,side),0,0);
                    break;
                case QUEEN:
                case ROOK:
                    if (c<7) for (int8_t cc = c+1; cc <  8; cc++){ MOVES_SLIDING_PIECE(r, cc, DOES_BREAK_CASTLE) }
                    if (c>0) for (int8_t cc = c-1; cc >= 0; cc--){ MOVES_SLIDING_PIECE(r, cc, DOES_BREAK_CASTLE) }
                    if (r<7) for (int8_t rr = r+1; rr <  8; rr++){ MOVES_SLIDING_PIECE(rr, c, DOES_BREAK_CASTLE) }
                    if (r>0) for (int8_t rr = r-1; rr >= 0; rr--){ MOVES_SLIDING_PIECE(rr, c, DOES_BREAK_CASTLE) }
                    if (state->pieces[r][c] == ROOK) break; //fall through if queen...
                case BISHOP:
                    for (int8_t d=1; d<=MIN(7-r,7-c); d++){ MOVES_SLIDING_PIECE(r+d,c+d,0) };
                    for (int8_t d=1; d<=MIN(7-r,  c); d++){ MOVES_SLIDING_PIECE(r+d,c-d,0) };
                    for (int8_t d=1; d<=MIN(  r,7-c); d++){ MOVES_SLIDING_PIECE(r-d,c+d,0) };
                    for (int8_t d=1; d<=MIN(  r,  c); d++){ MOVES_SLIDING_PIECE(r-d,c-d,0) };
                    break;
                case KNIGHT:
                    for (uint8_t i = 0; i < 8; i++){
                        if (ON_BOARD(r+horse_moves[i][0],c+horse_moves[i][1]) && 
                            side*state->pieces[r+horse_moves[i][0]][c+horse_moves[i][1]] <= 0)
                        CALL_ADD_MOVE(r+horse_moves[i][0],c+horse_moves[i][1],0,0,0);
                    } break;
                case KING:
                    for (uint8_t i = 0; i < 8; i++){
                        if (ON_BOARD(r+king_moves[i][0],c+king_moves[i][1]) &&
                            side*state->pieces[r+king_moves[i][0]][c+king_moves[i][1]] <= 0)
                        //only says that this makes a castle invalid if is currently valid
                        CALL_ADD_MOVE(r+king_moves[i][0],c+king_moves[i][1],0,0,
                                      !GET_BIT(state->invalid_castles,KINGSIDE_BIT(side)));
                    }
                    //castling
                    if (IS_EMPTY(BACK_ROW(side),5)&&IS_EMPTY(BACK_ROW(side),6)&&\
                        !GET_BIT(state->invalid_castles,KINGSIDE_BIT(side)))
                    CALL_ADD_MOVE(r,c+2,0,KINGSIDE,1);

                    if (IS_EMPTY(BACK_ROW(side),1)&&IS_EMPTY(BACK_ROW(side),2)&&IS_EMPTY(BACK_ROW(side),3)&&\
                        !GET_BIT(state->invalid_castles,QUEENSIDE_BIT(side)))
                    CALL_ADD_MOVE(r,c-2,0,QUEENSIDE,1);


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
