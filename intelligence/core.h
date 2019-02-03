#ifndef CORE_H
#define CORE_H

//TODO: implement macro GET_PIECE(r,c) => state->pieces[(r)][(c)]

//piece values
#define PAWN_VAL   (100)
#define ROOK_VAL   (500)
#define KNIGHT_VAL (350)
#define BISHOP_VAL (300)
#define QUEEN_VAL  (900)

//max score for check mates and stalemates
#define INFINITY (32767)

//number of pieces left before early game over
#define EARLY_GAME_PIECE_THRESH 24

//points per centre square
#define CENTRE_POWER (40)

//scaling factor for king position
//max influence is 4 * this
#define KING_POS_POWER (30)

//checks coordinate lies on board
#define ON_BOARD(x,y) ((x)>=0&&(x)<8&&(y)>=0&&(y)<8)

//gets distance of a coordinate-component from centre
#define CENTRE_DIST(c) (MIN(ABS(4-(c)),ABS(3-(c))))

//gets distance between two coordinates

//begin impure (hacky) #defines... (they assume variable names)

//sees if a piece is on your side
#define IS_MINE(r,c) (side*state->pieces[(r)][(c)]>0)
//why not just use !IS_MINE? because of empty squares; so...
#define IS_THEIRS(r,c) (side*state->pieces[(r)][(c)]<0)
//is that square empty?
#define IS_EMPTY(r,c) (state->pieces[(r)][(c)]==0)

//this just shortens the code for sliding pieces, used in in_check()
#define CHECK_SLIDING_PIECE(r,c,p) if (state->pieces[(r)][(c)] == -side*(p) || state->pieces[(r)][(c)] == -side*QUEEN) return 1; \
                                   if (state->pieces[(r)][(c)] != 0) break;

///calls add_move() with the appropriate variables; move.from assumed to be {r,c} and piece taken and move.to is {(i),(j)}
//(p) is whether move is a pawn_promote pawn promote
#define CALL_ADD_MOVE(i,j,p) add_move(state, moves_array, &num_moves, \
                                           (move_t){{r,c},{(i),(j)},state->pieces[(i)][(j)],(p)}, side)

//again this just shortens the generate_moves() code...  NOT A FUNCTION-LIKE MACRO, again
#define MOVES_SLIDING_PIECE(i,j) if (state->pieces[(i)][(j)] == 0){CALL_ADD_MOVE((i),(j),0);} \
                                   else if (IS_THEIRS((i),(j))){CALL_ADD_MOVE((i),(j),0);break;} \
                                   else break;

//...end impure hash_defines


uint8_t in_check(state_t *state, int8_t side);
uint8_t is_checkmated(state_t *state, int8_t side);
uint8_t generate_moves(state_t *state, int8_t side, move_t *moves_array);
void add_move(state_t *state, move_t *moves_array, uint8_t *moves, move_t move, uint8_t side);
int16_t evaluate(state_t *state);
int16_t negamax(state_t *state, move_t *best_move, int8_t side, uint8_t depth, int16_t alpha, int16_t beta);
int16_t order_moves(move_t *moves, uint8_t num_moves, uint8_t *order);
move_t  deepening_search(state_t *state, int8_t side, uint8_t time_limit_s);

#endif
