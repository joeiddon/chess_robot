import numpy as np
import recognition, intelligence

state = "RNBQKBNR/PPPPPPPP/        /        /        /        /pppppppp/rnbqkbnr/w/KQkg"
whites = np.array([[True]*8]*2 + [[False]*8]*6)
blacks = np.array([[False]*8]*6 + [[True]*8]*2)

side = "w"

while input("nothing to quit, something for next move"):
    newPos = recognition.recognise()[0 if side == "w" else 1]
    dif = newPos.astype('int') - (whites if side == "w" else blacks).astype('int')
    
    moved    = np.argwhere(dif < 0)
    appeared = np.argwhere(dif > 0)

    if len(moved) > 1:
        print("more than one piece moved")
        break
    elif len(moved) < 1:
        print("no pieces moved!")
        break
    if len(appeared) > 1:
        print("more than one piece appeared?!")
        break
    elif len(appeared) < 1:
        print("no pieces appeared")
        break

    move = [moved[0], appeared[0]]

    state = intelligence.makeMove(state, move)

    if side == "w":
        whites = newPos
    else:
        blacks = newPos

    side = "b" if side == "w" else "w"

    intelligence.display(state)

