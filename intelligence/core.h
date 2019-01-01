#ifndef CORE_H
#define CORE_H

//piece values
#define PAWN_VAL   (100)
#define ROOK_VAL   (500)
#define KNIGHT_VAL (350)
#define BISHOP_VAL (300)
#define QUEEN_VAL  (900)

//max score for check mates and stalemates
#define INFINITE_SCORE (32767)

//checks coordinate lies on board
#define ON_BOARD(x,y) ((x)>=0&&(x)<8&&(y)>=0&&(y)<8)

//NOT FUNCTION-LIKE MACRO (they assume variable names)
//sees if a piece is on your side
#define IS_MINE(r,c) (side*state->pieces[(r)][(c)]>0)
//why not just use !IS_MINE? because of empty squares; so...
#define IS_THEIRS(r,c) (side*state->pieces[(r)][(c)]<0)

//is the row a pawn-promote row for the given side?
#define IS_END_ROW(r,side) ((r)==((side)==WHITE?7:0))

//this just shortens the code for sliding pieces, used in in_check()  NOT A FUNCTION-LIKE MACRO, again
#define CHECK_SLIDING_PIECE(r,c,p) if (state->pieces[(r)][(c)] == -side*(p) || state->pieces[(r)][(c)] == -side*QUEEN) return 1; \
                                   if (state->pieces[(r)][(c)] != 0) break;

///calls add_move() with the appropriate variables - moving to (i,j). (p) is whether it is a pawn promote
#define CALL_ADD_MOVE(i,j,p) add_move(state, moves_array, &num_moves, (move_t){{r,c},{(i),(j)},(p),0}, side)

//again this just shortens the generate_moves() code...  NOT A FUNCTION-LIKE MACRO, again
#define MOVES_SLIDING_PIECE(i,c) if (state->pieces[(i)][(c)] == 0){CALL_ADD_MOVE((i),(c),0);} \
                                 else if (IS_THEIRS((i),(c))){CALL_ADD_MOVE((i),(c),0);break;} \
                                 else break; \


uint8_t in_check(state_t *state, int8_t side);
uint8_t is_checkmated(state_t *state, int8_t side);
uint8_t generate_moves(state_t *state, int8_t side, move_t *moves_array);
void add_move(state_t *state, move_t *moves_array, uint8_t *moves, move_t move, uint8_t side);
int16_t evaluate(state_t *state);

#endif
