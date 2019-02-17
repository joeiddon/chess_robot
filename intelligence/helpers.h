#ifndef HELPERS_H
#define HELPERS_H

//really hacky...
#define MOVE_TO move->to[0]][move->to[1]
#define MOVE_FROM move->from[0]][move->from[1]

void print_state(state_t *state);
void print_move(move_t* move);
uint16_t get_time_s();
uint16_t str_to_int(uint8_t *string);
void make_move(state_t *state, move_t *move);
void inverse_move(state_t *state, move_t *move);
move_t get_user_move_instance(state_t *state, uint8_t fr, uint8_t fc, uint8_t tr, uint8_t tc);

void print_negamax_route(state_t *state, move_t *best_move, int8_t side, uint8_t depth);

#ifdef DEBUG_NEGAMAX
void copy_state(state_t *in_state, state_t *copy_state);
#endif

#endif
