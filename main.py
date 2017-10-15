import numpy as np
import recognition, intelligence

state = [list(r) for r in "RNBQKBNR/PPPPPPPP/        /        /        /        /pppppppp/rnbqkbnr/KQkg".split("/")]

whites = np.array([[True]*8]*2 + [[False]*8]*6)
blacks = np.array([[False]*8]*6 + [[True]*8]*2)

side = "w"

while True:
    input("enter when made move")

    newPos = recognition.recognise()[0 if side == "w" else 1]
    dif = newPos.astype('int') - (whites if side == "w" else blacks).astype('int')
    
    intelligence.display(dif.astype("str"))

    moved    = np.argwhere(dif < 0)
    appeared = np.argwhere(dif > 0)

    if len(moved) > 1:
        print("more than one piece moved")
        continue
    elif len(moved) < 1:
        print("no pieces moved!")
        continue
    if len(appeared) > 1:
        print("more than one piece appeared?!")
        continue
    elif len(appeared) < 1:
        print("no pieces appeared")
        continue

    move = [moved[0], appeared[0]]

    state = intelligence.makeMove(state, move)

    if side == "w":
        whites = newPos
    else:
        blacks = newPos

    intelligence.display(state)

    print("\nmy move makes it:")

    state = intelligence.makeMove(state, intelligence.bestMove(state, "b", 3))

    intelligence.display(state)
