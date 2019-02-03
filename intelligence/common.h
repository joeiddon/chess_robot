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

//flips case of character
#define CHANGE_CASE(c) ((c)^32)
//is the character upper case?
#define IS_LOWER(c) ((c) >> 5 & 1)

//whats the maximum number of moves at a give state?
#define MAX_NUM_MOVES (100)

//get index of back row, given side
#define BACK_ROW(side) ((side)==WHITE?0:7)
//is this row a back row?
#define IS_PAWN_ROW(r,side) ((r)==((side)==WHITE?1:6))

//debug options, comment in for debugging
//#define DEBUG_NEGAMAX

//see README.md for better explanation of these strutures

typedef struct {
    uint8_t from[2];
    uint8_t to[2];
    int8_t  piece_taken; //what piece will be taken? 0 for none of course
    uint8_t is_pawn_promotion; //just 1 or 0; promotion assumed to be queen
} move_t;

typedef struct {
    //array of piece codes
    int8_t pieces[8][8];
} state_t;

#endif
