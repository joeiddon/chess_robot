#ifndef HELPERS_H
#define HELPERS_H

//really hacky...
#define MOVE_TO move->to[0]][move->to[1]
#define MOVE_FROM move->from[0]][move->from[1]

void print_state(state_t *state);
void make_move(state_t *state, move_t *move);
void inverse_move(state_t *state, move_t *move);
void print_move(move_t* move);

#endif
