#ifndef COMMON_H
#define COMMON_H

#define PAWN   (1)
#define ROOK   (2)
#define KNIGHT (3)
#define BISHOP (4)
#define QUEEN  (5)
#define KING   (6)

#define WHITE  (1)
#define BLACK  (-1)

#define ABS(x) ((x)<0?-(x):(x))
#define MIN(x,y) ((x)<(y)?(x):(y))

//converts lower case character to upper case
#define UPPER(c) ((c)^32)

//whats the maximum number of moves at a give state?
#define MAX_NUM_MOVES (100)

//castling
#define KINGSIDE  1
#define QUEENSIDE 2

typedef struct {
    uint8_t from[2];
    uint8_t to[2];
    uint8_t is_pawn_promotion; //just 1 or 0, promotion assumed to be queen
    uint8_t castle; //move postions (from,to) are that of king
} move_t;

typedef struct {
    //array of piece codes
    int8_t pieces[8][8];
    //piece code of last taken for inverse_move function
    int8_t last_taken;
} state_t;

#endif
