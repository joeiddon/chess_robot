chess_ai
========================

#### A Chess AI written in pure C. [work in progress]

An AI was originally written for this project in Python, however I made the mistake of describing my board state via a list which of course is mutable. As a result, I was forced to make lots of copies of the board state which severely reduced the speed of computation; resulting in an easily beatable player. Thus, I decided to start again from scratch at a lower level. This is that attempt.

---

### Navigating the Source

The code is split up into three `.c` source files: **`main.c`**, **`core.c`** and **`helpers.c`**.

 - The `main()` function is appropriately located in `main.c` and at the time of writing, merely has some different tests that can be commented in or out.
 - The `core.c` file contains the most algorithmically and computationally heavy functions (e.g. `generate_moves()` and `evaluate()`).
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

In **`state_t`**, the state of the board is described by an `8` by `8` array of bytes (`int8_t`) named `pieces`. The array is described from white's perspective, with `(0,0)` at white's left castle. The magnitude of each element in the array determines the piece type, e.g. a bishop is represented by a `4` and the sign represents the side, positive for white and negative for black. An empty square is delineated by a zero. Their is also a member called `invalid_castles` which is a byte (`uint8_t`). It encodes which of the four possible castling moves are valid through its four least significant bits. Then, using bit-wise operators I wrote three macros to manipulate this variables bits: `GET_BIT(x,i)`, `SET_BIT_HIGH(x,i)` and `SET_BIT_LOW(x,i)`. Using these, we can now read and write to this member - resulting in a memory-efficient method to store which castling moves are valid.

The **`move_t`** structure has five members: `from`, `to`, `piece_taken`, `is_pawn_promotion`, `castle` and `makes_castle_invalid`. The `from` and `to` members are byte arrays (`uint8_t`), each of length 2. They hold the coordinates of the position the piece moved was picked up from, and placed at, respectively. The `piece_taken` member is a signed byte representing the piece taken with that move, if the move does result in a piece being taken, otherwise it holds `0`. This is required for the `inverse_move()` function, so the correct piece can be put back on the board. The `is_pawn_promotion` member is merely a boolean whose meaning is evident from its name. This member is again necessary for the `inverse_move` function to distinguish between a queen being moved by one vertically onto the back row and a legit pawn promotion. The `castle` member depicts whether the move is a castle, with a `0` implying not, a `1` implying a kingside castle and a `2` implying a queenside castle. Finally, the `makes_castle_invalid` member is a boolean representing if that move makes a castling no longer valid (illegal). You may ask: why is this necessary when you can just say in the `make_move()` function that whenever the king or a castle moves, the corresponding castle move(s) is/are now no longer valid? As before, this member is necessary for the `inverse_move()` function as in the case where the move describes a king or castle moving away from its home spot, it needs to be able to tell if this was the first move of that piece, or if it is merely a move later in the game when the piece re-visits the home spot. Hence, to be able to decide whether undoing this move makes it now legal to castle, the member is required to tell it if that specific move was the one that made it illegal (very long-winded, I know).

Now those two structures have been explained (in much detail), the code should be relatively easy to follow. Note that the `in_check` and `generate_moves` functions both use hacky macros, defined in `core.h`, that contain if-statements and `break` statements and span over multiple lines. They make those functions take at least half the line length they would be without them, but at the cost of making the code slightly unclear if you wish to follow what they do. Similarly, in the `make_move()` and `inverse_make()` functions, the macros `MOVE_TO` and `MOVE_FROM`, defined in `helpers.h`, make the code readable but are quite impure (hacky).

However, if I were to delve into the core of the code, I must explain the [Negamax algorithm](https://en.wikipedia.org/wiki/Negamax) and [Alpha-Beta pruning](https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning).

To explain negamax, un understanding of [minimax](https://en.wikipedia.org/wiki/Minimax) is required. This is an algorithm for deciding what is the best move to play in a given game. It works by depth-first searching the game tree, at each node, depending on whose turn it is, the algorithm tries to maximise or minimise the best score of all the moves that can be played from there. Since the game tree for chess cannot be traversed (the branching factor is `~30` and say that a game takes `~40` moves for each player, then there are `~30^80` possible states which is `~10^120` and there are only `~10^20` atoms in the observable universe), we must use an evaluation function to "rate" the current game position for a certain side once a given depth has been reached.

Negamax merely takes advantage of the fact that `max(x,y) = -min(-x,-y)`, so instead of having to write an if-statement to decide whether we should be maximising or minimising at a given stage, we can just always maximise the negative of the different move scores to decide our best move. This makes the basic algorithm really quite simple.

In pseudo-code:

```
If depth is 0:
    Return evaluation of current board

Otherwise...

Set our best move score to -infinity.
Loop through all possible moves
    Make that move
    If - opponents best score > best score:
        Set best move score to - opponents best score
        Set best move to current move
    Undo that move
End Loop

Return best move score
[...and somehow propagate up the corresponding best move]
```

Finally, there is room for a huge efficiency improvement. It's name, as previously mentioned, is alpha-beta pruning. I'll admit to it taking a while for me to fully understand its implementation but, with some thought, the following diagram should make it evident how certain portions of the tree can be "discounted" (not searched) - thus leading to less computation; allowing us to search to greater depths.

![negamax and alpha-beta pruning image](https://upload.wikimedia.org/wikipedia/commons/thumb/9/91/AB_pruning.svg/600px-AB_pruning.svg.png)
*[source](https://commons.wikimedia.org/wiki/File:AB_pruning.svg)*

An example may also help. Say we have 2 moves to choose from: A and B. In doing move A, we see that our opponents best move ends up in us losing a pawn. Then mid-way though evaluating move B, we see that one of our opponents responses ends with us being a queen down. We now don't need to bother evaluating their other responses as we know that one of their replies results in an outcome worse than our current best outcome that we are assured of (just losing a pawn). Hence we don't need to evaluate these replies and can "prune" the game tree (all the two red lines in the diagram above).

Therefore, whenever we can assure that our best outcome is better than their best outcome (our worst), we can stop evaluating the current moves and return our best outcome.

This is coded by holding two variables: `alpha` and `beta` which represent our best and their best outcomes respectively. Then, we can say that whenever `alpha >= beta`, we can stop looping through our different moves (`break`/`return`). Since we are using negamax, when we pass these down to the next `negamax`, we must reverse them around - i.e. our best score is now the opponent's "opponent's best score" and their best score is now the opponent's "best score". Finally, we must negate both these values because they are trying to maximise their score.

This method leads to a major perfomance game. In the mid-game, the engine can search to depth `5` from the initial state in only about `1` second.

However, a very simple addition can be made to give us further advantages. This stems from the fact that the more of the tree we can prune earlier on, the faster are search is. We can prune more tree earlier on by doing some initial move ordering - taking a guess at what will be a good move. This function could be improved, but my current function `order_moves()` simply tells negamax which moves take pieces and which don't (very simple given our `move_t` data structure). Using this, these capture moves can be searched first - making prunes possible earlier.

The result of this initial move ordering is dramatic. It can now search to depth `6` from a complicated state (lots of captures, very congested) in just `1.5` seconds where the old algorithm took roughly `12` seconds for this!

My implementation for the negamax algorithm along with the simplistic `order_moves()` function can be found in **`core.c`**

### Limitations:

- No En-Passant.
- Auto queen-promotion for all pawns.
