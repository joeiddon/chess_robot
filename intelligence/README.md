chess_ai
========================

#### A Chess AI written in pure C. [work in progress]

An AI was orginally written for this project in Python, however I made the mistake of describing my board state via a list which of course is mutable. As a result, I was forced to make lots of copies of the board state which severely reduced the speed of computation; resulting in an easily beatable player. Thus I decided to start again from scratch at a lower level. This is that attempt.

---

### Navigating the Source

The code is split up into three `.c` source files: **`main.c`**, **`core.c`** and **`helpers.c`**.

 - The `main()` function is appropriately located in `main.c` and at the time of writing, merely has some different tests that can be commented in or out.
 - The `core.c` file contains the most algorithmically and computationally heavy funcitons (e.g. `generate_moves()` and `evaluate()`).
 - Handy functions, such as `display_state()` and `make_move()`, are found in `helpers.c`.

In addition to these, there are also three header files: **`common.h`**, **`core.h`** and **`helpers.h`**. The `common.h` header is `#include`d in every source file and contains `#define`s required by lots of functions and the main `struct`s. The `core.h` and `helpers.h` files are only included in their respective `.c` source files and the `main.c` source file.

---

### Compilation and Usage

To compile, use the following command.

```
gcc *.c -o chess_ai
```

The program can then be run with: `./chess_ai`.

---

### Code Explanation:

There are two main types (structures): `state_t` and `move_t` used throughout the code. (I appended the `_t` to make distinguish them from variable names).

In **`state_t`**, the state of the board is described by an `8` by `8` array of bytes (`int8_t`) named `pieces`. The array is described from white's perspective, with `(0,0)` at white's left castle. The magnitude of each element in the array determines the piece type, e.g. a bishop is represented by a `4` and the sign represents the side, positive for white and negative for black. An empty square is delineated by a zero.

Another member of this structure, `last_taken`, holds the piece code (as described above) of the piece that was taken last move if a piece was taken last move, otherwise it should just be set to `0`. This is necessary for the `inverse_move` function that undos a given move.

In the future, more members will be added to this structure, such as one to represent whether the king has moved yet which will be required to know if we can castle.

The **`move_t`** structure has four members: `from`, `to`, `is_pawn_promotion` and `castle`. The `from` and `to` members are char arrays (`uint8_t`), each of length 2. They hold the coordinates of the position the piece moved was picked up from, and placed at, respectively. The `is_pawn_promotion` is merely a boolean whose meaning is evident from its name. This member is again necessary for the `inverse_move` function to distinguish between a queen being moved by one verically onto the back row and a legit pawn promotion. Finally, the `castle` member depicts whether the move is a castle, with a `0` implying not, a `1` implying a kingside castle and a `2` implying a queenside castle.

Now those two structures have been explained, the code should be relatively easy to follow. Note that the `in_check` and `generate_moves` functions both use hacky macros, defined in `core.h`, that contain if-statements and `break` statements and span over multiple lines. They make those functions take at least half the line length they would be without them, but at the cost of making the code slightly unclear if you wish to follow what they do. Similarly, in the `make_move()` and `inverse_make()` functions, the macros `MOVE_TO` and `MOVE_FROM`, defined in `helpers.h`, make the code readable but are quite impure (hacky).

---

### Limitations:

- No En-Passant.
- Auto queen-promotion for all pawns.
