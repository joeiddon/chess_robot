#ifndef COMMON_H
#define COMMON_H

//piece encodings
#define PAWN   (1)
#define ROOK   (2)
#define KNIGHT (3)
#define BISHOP (4)
#define QUEEN  (5)
#define KING   (6)

//side encodings
#define WHITE  ( 1)
#define BLACK  (-1)

//used for finding type of signed piece
#define ABS(x) ((x)<0?-(x):(x))
//used in calculating diaganol moves
#define MIN(x,y) ((x)<(y)?(x):(y))

//converts lower case character to upper case
#define UPPER(c) ((c)^32)

//whats the maximum number of moves at a give state?
#define MAX_NUM_MOVES (100)

//get index of back row, given side
#define BACK_ROW(side) ((side)==WHITE?0:7)

//castling macros
//bitwise operators for use on state_t.castling_pieces
#define GET_BIT(x,i) ((x) >> (i) & 1)
#define SET_BIT_HIGH(x,i) x |= (1<<i)
#define SET_BIT_LOW(x,i)  x &= ~(1<<i)
//bit meanings in state_t.sides_can_castle
#define BIT_WHITE_KINGSIDE  (0)
#define BIT_WHITE_QUEENSIDE (1)
#define BIT_BLACK_KINGSIDE  (2)
#define BIT_BLACK_QUEENSIDE (3)
//gets appropriate bit for side
#define KINGSIDE_BIT(side) ((side)==WHITE?BIT_WHITE_KINGSIDE:BIT_BLACK_KINGSIDE)
#define QUEENSIDE_BIT(side) ((side)==WHITE?BIT_WHITE_QUEENSIDE:BIT_BLACK_QUEENSIDE)

//castling
#define KINGSIDE  (1)
#define QUEENSIDE (2)

//see README.md for better explanation of these strutures

typedef struct {
    uint8_t from[2];
    uint8_t to[2];
    uint8_t is_pawn_promotion; //just 1 or 0; promotion assumed to be queen
    uint8_t castle; //KINGSIDE or QUEENSIDE; move postions (from,to) are that of king
    uint8_t makes_castle_invalid; //does it make a castle invalid? (for inverse_move)
} move_t;

typedef struct {
    //array of piece codes
    int8_t pieces[8][8];
    //piece code of last taken for inverse_move function
    int8_t last_taken;
    //bits 0,1,2,3 represent which castle moves are invalid
    //see #defines above for which bit represents which castle type
    uint8_t invalid_castles;
} state_t;

#endif
