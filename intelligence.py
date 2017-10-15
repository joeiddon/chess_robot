"""
Chess AI library
"""

state = [list(r) for r in "RNBQKBNR/PPPPPPPP/        /        /        /        /pppppppp/rnbqkbnr/KQkg".split("/")]

def time(expr, precision):
    import time
    start = time.time()
    for _ in range(precision):
        eval(expr)
    return str(((time.time() - start)/precision)*1000) + "ms"

def bestMove(state, side, depth):
    return negamaxItBuddy(state, depth, -99999999, 99999999, side)[1]
    
def negamaxItBuddy(state, depth, alpha, beta, side):
    if depth == 0:
        return [evaluate(state, side)]
    
    moves = availableMoves(state, side)
    
    if not len(moves):
        return [99999999]

    bestScore = -99999999
    
    for move in moves:
        score = -negamaxItBuddy(makeMove(state, move), depth-1, -beta, -alpha, "b" if side == "w" else "w")[0]
        if score > bestScore:
            bestScore = score
            bestMove = move
        alpha = max(alpha, score)
        if alpha >= beta:
            break   #pruuunne
    return [bestScore, bestMove]

def evaluate(state, side):
    score = 0
    spaces = 0

    #material
    for r in range(8):
        for c in range(8):
            piece = state[r][c]
            if piece == " ":
                spaces += 1
            if piece == "Q":
                score += 9
            elif piece == "q":
                score -= 9
            elif piece == "R":
                score += 5
            elif piece == "r":
                score -= 5
            elif piece == "B" or piece == "N":
                score += 3
            elif piece == "b" or piece == "n":
                score -= 3
            elif piece == "P":
                score += 1
            elif piece == "p":
                score -= 1

    #development in beggining
    if  spaces < 40: #32 spaces at beggining as get taken, increases
        score += 0.1 * (state[0].count(' ') - state[7].count(' '))
    
    #mobility
    whiteMoves = len(availableMoves(state, "w"))
    blackMoves = len(availableMoves(state, "b"))
    
    score += 0.01 * (whiteMoves - blackMoves)
    
    #if white is checkmated, score drops to -infinity if black checkmated, score goes to infinity
    if whiteMoves == 0:
        if inCheck(state, "w"):                             #white is checmated
            score -= 99999999
        else:                                               #white is stalemated
            score += 99999999 * (1 if side == "w" else -1)  #make score crap for both sides
    elif blackMoves == 0:                               #black is checkmated
        if inCheck(state,"b"):
            score += 99999999
        else:                                               #balck is stalemated
            score += 99999999 * (1 if side == "w" else -1)  #make score crap for both sides
    
    return score * (1 if side == "w" else -1)
    
def inCheck(state, side):
    for r in range(8):
        for c in range(8):
            if state[r][c] != ("K" if side == "w" else "k"):
                continue
            rowAhead = r + (1 if side == "w" else -1)
            if c > 0 and state[rowAhead][c-1] == ("p" if side == "w" else "P"):
                return True
            if c < 7 and state[rowAhead][c+1] == ("p" if side == "w" else "P"):
                return True
            for file in range(c+1, 8):
                if state[r][file] == ("r" if side == "w" else "R") or state[r][file] == ("q" if side == "w" else "Q"):
                    return True
                if state[r][file] != " ":
                    break
            for file in range(c-1, -1, -1):
                if state[r][file] == ("r" if side == "w" else "R") or state[r][file] == ("q" if side == "w" else "Q"):
                    return True
                if state[r][file] != " ":
                    break
            for rank in range(r+1, 8):
                if state[rank][c] == ("r" if side == "w" else "R") or state[rank][c] == ("q" if side == "w" else "Q"):
                    return True
                if state[rank][c] != " ":
                    break
            for rank in range(r-1, -1, -1):
                if state[rank][c] == ("r" if side == "w" else "R") or state[rank][c] == ("q" if side == "w" else "Q"):
                    return True
                if state[rank][c] != " ":
                    break
            for d in range(1, min(7-r,7-c)+1):
                if state[r+d][c+d] == ("b" if side == "w" else "B") or state[r+d][c+d] == ("q" if side == "w" else "Q"):
                    return True
                if state[r+d][c+d] != " ":
                    break
            for d in range(1, min(7-r,c)+1):
                if state[r+d][c-d] == ("b" if side == "w" else "B") or state[r+d][c-d] == ("q" if side == "w" else "Q"):
                    return True
                if state[r+d][c-d] != " ":
                    break
            for d in range(1, min(r,7-c)+1):
                if state[r-d][c+d] == ("b" if side == "w" else "B") or state[r-d][c+d] == ("q" if side == "w" else "Q"):
                    return True
                if state[r-d][c+d] != " ":
                    break
            for d in range(1, min(r,c)+1):
                if state[r-d][c-d] == ("b" if side == "w" else "B") or state[r-d][c-d] == ("q" if side == "w" else "Q"):
                    return True
                if state[r-d][c-d] != " ":
                    break
            pnts = [p for p in [[r+1,c+2], [r+1,c-2], [r-1,c+2], [r-1,c-2], [r+2,c+1], [r-2,c+1], [r+2,c-1], [r-2,c-1]] if p[0] >= 0 and p[1] >= 0 and p[0] < 8 and p[1] < 8]
            for p in pnts:
                if state[p[0]][p[1]] == ("n" if side == "w" else "N"):
                    return True
            pnts = [p for p in [[r+1,c], [r+1,c+1], [r, c+1], [r-1,c+1], [r-1,c], [r-1,c-1], [r,c-1], [r+1,c-1]] if p[0] >= 0 and p[1] >= 0 and p[0] < 8 and p[1] < 8]
            for p in pnts:
                if state[p[0]][p[1]] == ("k" if side == "w" else "K"):
                    return True
            return False
                    
def availableMoves(state, side):
    moves = []
    for r in range(8):
        for c in range(8):
            if state[r][c] == " " or (state[r][c].islower() if side == "w" else state[r][c].isupper()):
                continue
            piece = state[r][c].lower()
            if piece == "p":
                rowAhead = r + (1 if side == "w" else -1)
                row2Ahead = r + (2 if side == "w" else -2)
                if state[rowAhead][c] == " ":
                    moves.append([[r,c], [rowAhead, c]])
                    if r == (1 if side == "w" else 6) and state[row2Ahead][c] == " ":
                        moves.append([[r,c], [row2Ahead, c]])
                if c > 0 and state[rowAhead][c-1] != " " and (state[rowAhead][c-1].islower() if side == "w" else state[rowAhead][c-1].isupper()):
                    moves.append([[r,c], [rowAhead,c-1]])
                if c < 7 and state[rowAhead][c+1] != " " and (state[rowAhead][c+1].islower() if side == "w" else state[rowAhead][c-1].isupper()):
                    moves.append([[r,c], [rowAhead,c-1]])
            elif piece == "r" or piece == "q":
                for file in range(c+1, 8):
                    if state[r][file] == " ":
                        moves.append([[r,c], [r,file]])
                    elif (state[r][file].islower() if side == "w" else state[r][file].isupper()):
                        moves.append([[r,c], [r,file]])
                        break
                    else: #must be of same colour
                        break
                for file in range(c-1, -1, -1):
                    if state[r][file] == " ":
                        moves.append([[r,c], [r,file]])
                    elif (state[r][file].islower() if side == "w" else state[r][file].isupper()):
                        moves.append([[r,c], [r,file]])
                        break
                    else: #must be of same colour
                        break
                for rank in range(r+1, 8):
                    if state[rank][c] == " ":
                        moves.append([[r,c], [rank,c]])
                    elif (state[rank][c].islower() if side == "w" else state[rank][c].isupper()):
                        moves.append([[r,c], [rank,c]])
                        break
                    else: #must be of same colour
                        break
                for rank in range(r-1, -1, -1):
                    if state[rank][c] == " ":
                        moves.append([[r,c], [rank,c]])
                    elif (state[rank][c].islower() if side == "w" else state[rank][c].isupper()):
                        moves.append([[r,c], [rank,c]])
                        break
                    else: #must be of same colour
                        break
            elif piece == "b" or piece == "q":
                for d in range(1, min(7-r,7-c)+1):
                    if state[r+d][c+d] == " ":
                        moves.append([[r,c], [r+d,c+d]])
                    elif (state[r+d][c+d].islower() if side == "w" else state[r+d][c+d].isupper()):
                        moves.append([[r,c], [r+d,c+d]])
                        break
                    else: #must be of same colour
                        break
                for d in range(1, min(7-r,c)+1):
                    if state[r+d][c-d] == " ":
                        moves.append([[r,c], [r+d,c-d]])
                    elif (state[r+d][c-d].islower() if side == "w" else state[r+d][c-d].isupper()):
                        moves.append([[r,c], [r+d,c-d]])
                        break
                    else: #must be of same colour
                        break
                for d in range(1, min(r,7-c)+1):
                    if state[r-d][c+d] == " ":
                        moves.append([[r,c], [r-d,c+d]])
                    elif (state[r-d][c+d].islower() if side == "w" else state[r-d][c+d].isupper()):
                        moves.append([[r,c], [r-d,c+d]])
                        break
                    else: #must be of same colour
                        break
                for d in range(1, min(r,c)+1):
                    if state[r-d][c-d] == " ":
                        moves.append([[r,c], [r-d,c-d]])
                    elif (state[r-d][c-d].islower() if side == "w" else state[r-d][c-d].isupper()):
                        moves.append([[r,c], [r-d,c-d]])
                        break
                    else: #must be of same colour
                        break               
            elif piece == "n":
                pnts = [p for p in [[r+1,c+2], [r+1,c-2], [r-1,c+2], [r-1,c-2], [r+2,c+1], [r-2,c+1], [r+2,c-1], [r-2,c-1]] if p[0] >= 0 and p[1] >= 0 and p[0] < 8 and p[1] < 8]
                for p in pnts:
                    if state[p[0]][p[1]] == " ":
                            moves.append([[r,c], p])
                    elif (state[p[0]][p[1]].islower() if side == "w" else state[p[0]][p[1]].isupper()):
                            moves.append([[r,c], p])
            elif piece == "k":
                pnts = [p for p in [[r+1,c], [r+1,c+1], [r, c+1], [r-1,c+1], [r-1,c], [r-1,c-1], [r,c-1], [r+1,c-1]] if p[0] >= 0 and p[1] >= 0 and p[0] < 8 and p[1] < 8]
                for p in pnts:
                    if state[p[0]][p[1]] == " ":
                            moves.append([[r,c], p])
                    elif (state[p[0]][p[1]].islower() if side == "w" else state[p[0]][p[1]].isupper()):
                            moves.append([[r,c], p])

    #castling stuf
    kingPos = [0 if side == "w" else 7, 4]
    if ("K" if side == "w" else "k") in state[-1] and state[kingPos[0]][kingPos[1]] == ("K" if side == "w" else "k") and state[kingPos[0]][5] == " " and state[kingPos[0]][6] == " " and not inCheck(makeMove(state,[kingPos, [kingPos[0], 5]]), side):
        moves.append([kingPos, [kingPos[0], 6]])
    if ("Q" if side == "w" else "q") in state[-1] and state[kingPos[0]][kingPos[1]] == ("K" if side == "w" else "k") and state[kingPos[0]][3] == " " and state[kingPos[0]][2] == " " and state[kingPos[0]][1] and not inCheck(makeMove(state,[kingPos, [kingPos[0], 3]]), side) and not inCheck(makeMove(state,[kingPos, [kingPos[0], 2]]), side):
        moves.append([kingPos, [kingPos[0], 2]])

    moves = [m for m in moves if not inCheck(makeMove(state, m), side)]
    return moves

def display(state):
    for row in reversed(state):
        print("".join(row))

def makeMove(state, move):
    state = [r[:] for r in state]
    state[move[1][0]][move[1][1]] = state[move[0][0]][move[0][1]]
    state[move[0][0]][move[0][1]] = " "
    
    #pawn promote
    if move[1][0] == 7 and state[move[1][0]][move[1][1]] == "P":
        state[7][move[1][1]] = "Q"
    elif move[1][0] == 0 and state[move[1][0]][move[1][1]] == "p":
        state[0][move[1][1]] = "q"

    #castling stuf
    if move[0][0] == 0 and move[0][1] == 4:
        if move[1][0] == 0 and move[1][1] == 6:
            state = makeMove(state, [[0,7], [0,5]])
            state[-1].remove("K")
        elif move[1][0] == 0 and move[1][1] == 2:
            state = makeMove(state, [[0,0], [0,3]])
            state[-1].remove("Q")
    elif move[0][0] == 7 and move[0][1] == 4:
        if move[1][0] == 7 and move[1][1] == 6:
            state = makeMove(state, [[7,7], [7,5]])
            state[-1].remove("k")
        elif move[1][0] == 7 and move[1][1] == 2:
            state = makeMove(state, [[7,0], [7,3]])
            state[-1].remove("q")

    return state
