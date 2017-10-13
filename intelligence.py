"""
Chess AI library
"""

state = "RNBQKBNR/PPPPPPPP/        /        /        /        /pppppppp/rnbqkbnr/w/KQkg"

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
			break	#pruuunne
	return [bestScore, bestMove]

def evaluate(state, side):
	score = 0
	
	#material
	score += 9 * (state[:72].count('Q') - state[:72].count('q'))
	score += 5 * (state[:72].count('R') - state[:72].count('r'))
	score += 3 * (state[:72].count('B') - state[:72].count('b'))
	score += 3 * (state[:72].count('N') - state[:72].count('n'))
	score += 1 * (state[:72].count('P') - state[:72].count('p'))
				
	#development in beggining
	if  state[:72].count(' ') < 44:	#32 spaces at beggining as get taken, increases
		score += 0.1 * (state[:8].count(' ') - state[63:71].count(' '))
	
	#mobility
	whiteMoves = len(availableMoves(state, "w"))
	blackMoves = len(availableMoves(state, "b"))
	
	score += 0.01 * (whiteMoves - blackMoves)
	
	#if white is checkmated, score drops to -infinity if black checkmated, score goes to infinity
	if whiteMoves == 0:
		if inCheck(state, "w"):					   			#white is checmated
			score -= 99999999
		else: 												#white is stalemated
			score += 99999999 * (1 if side == "w" else -1)	#make score crap for both sides
	elif blackMoves == 0: 								#black is checkmated
		if inCheck(state,"b"):
			score += 99999999
		else:												#balck is stalemated
			score += 99999999 * (1 if side == "w" else -1)  #make score crap for both sides
	
	return score * (1 if side == "w" else -1)
	
def inCheck(state, side):
	board = state.split("/")[:8]
	for r in range(8):
		for c in range(8):
			if board[r][c] != ("K" if side == "w" else "k"):
				continue
			rowAhead = r + (1 if side == "w" else -1)
			if c > 0 and board[rowAhead][c-1] == ("p" if side == "w" else "P"):
				return True
			if c < 7 and board[rowAhead][c+1] == ("p" if side == "w" else "P"):
				return True
			for file in range(c+1, 8):
				if board[r][file] == ("r" if side == "w" else "R") or board[r][file] == ("q" if side == "w" else "Q"):
					return True
				if board[r][file] != " ":
					break
			for file in range(c-1, -1, -1):
				if board[r][file] == ("r" if side == "w" else "R") or board[r][file] == ("q" if side == "w" else "Q"):
					return True
				if board[r][file] != " ":
					break
			for rank in range(r+1, 8):
				if board[rank][c] == ("r" if side == "w" else "R") or board[rank][c] == ("q" if side == "w" else "Q"):
					return True
				if board[rank][c] != " ":
					break
			for rank in range(r-1, -1, -1):
				if board[rank][c] == ("r" if side == "w" else "R") or board[rank][c] == ("q" if side == "w" else "Q"):
					return True
				if board[rank][c] != " ":
					break
			for d in range(1, min(7-r,7-c)+1):
				if board[r+d][c+d] == ("b" if side == "w" else "B") or board[r+d][c+d] == ("q" if side == "w" else "Q"):
					return True
				if board[r+d][c+d] != " ":
					break
			for d in range(1, min(7-r,c)+1):
				if board[r+d][c-d] == ("b" if side == "w" else "B") or board[r+d][c-d] == ("q" if side == "w" else "Q"):
					return True
				if board[r+d][c-d] != " ":
					break
			for d in range(1, min(r,7-c)+1):
				if board[r-d][c+d] == ("b" if side == "w" else "B") or board[r-d][c+d] == ("q" if side == "w" else "Q"):
					return True
				if board[r-d][c+d] != " ":
					break
			for d in range(1, min(r,c)+1):
				if board[r-d][c-d] == ("b" if side == "w" else "B") or board[r-d][c-d] == ("q" if side == "w" else "Q"):
					return True
				if board[r-d][c-d] != " ":
					break
			pnts = [p for p in [[r+1,c+2], [r+1,c-2], [r-1,c+2], [r-1,c-2], [r+2,c+1], [r-2,c+1], [r+2,c-1], [r-2,c-1]] if p[0] >= 0 and p[1] >= 0 and p[0] < 8 and p[1] < 8]
			for p in pnts:
				if board[p[0]][p[1]] == ("n" if side == "w" else "N"):
					return True
			pnts = [p for p in [[r+1,c], [r+1,c+1], [r, c+1], [r-1,c+1], [r-1,c], [r-1,c-1], [r,c-1], [r+1,c-1]] if p[0] >= 0 and p[1] >= 0 and p[0] < 8 and p[1] < 8]
			for p in pnts:
				if board[p[0]][p[1]] == ("k" if side == "w" else "K"):
					return True
			return False
					
def availableMoves(state, side):
	board = state.split("/")[:8]
	moves = []
	for r in range(8):
		for c in range(8):
			if board[r][c] == " " or (board[r][c].islower() if side == "w" else board[r][c].isupper()):
				continue
			piece = board[r][c].lower()
			if piece == "p":
				rowAhead = r + (1 if side == "w" else -1)
				row2Ahead = r + (2 if side == "w" else -2)
				if board[rowAhead][c] == " ":
					moves.append([[r,c], [rowAhead, c]])
					if r == (1 if side == "w" else 6) and board[row2Ahead][c] == " ":
						moves.append([[r,c], [row2Ahead, c]])
				if c > 0 and board[rowAhead][c-1] != " " and (board[rowAhead][c-1].islower() if side == "w" else board[rowAhead][c-1].isupper()):
					moves.append([[r,c], [rowAhead,c-1]])
				if c < 7 and board[rowAhead][c+1] != " " and (board[rowAhead][c+1].islower() if side == "w" else board[rowAhead][c-1].isupper()):
					moves.append([[r,c], [rowAhead,c-1]])
			elif piece == "r" or piece == "q":
				for file in range(c+1, 8):
					if board[r][file] == " ":
						moves.append([[r,c], [r,file]])
					elif (board[r][file].islower() if side == "w" else board[r][file].isupper()):
						moves.append([[r,c], [r,file]])
						break
					else: #must be of same colour
						break
				for file in range(c-1, -1, -1):
					if board[r][file] == " ":
						moves.append([[r,c], [r,file]])
					elif (board[r][file].islower() if side == "w" else board[r][file].isupper()):
						moves.append([[r,c], [r,file]])
						break
					else: #must be of same colour
						break
				for rank in range(r+1, 8):
					if board[rank][c] == " ":
						moves.append([[r,c], [rank,c]])
					elif (board[rank][c].islower() if side == "w" else board[rank][c].isupper()):
						moves.append([[r,c], [rank,c]])
						break
					else: #must be of same colour
						break
				for rank in range(r-1, -1, -1):
					if board[rank][c] == " ":
						moves.append([[r,c], [rank,c]])
					elif (board[rank][c].islower() if side == "w" else board[rank][c].isupper()):
						moves.append([[r,c], [rank,c]])
						break
					else: #must be of same colour
						break
			elif piece == "b" or piece == "q":
				for d in range(1, min(7-r,7-c)+1):
					if board[r+d][c+d] == " ":
						moves.append([[r,c], [r+d,c+d]])
					elif (board[r+d][c+d].islower() if side == "w" else board[r+d][c+d].isupper()):
						moves.append([[r,c], [r+d,c+d]])
						break
					else: #must be of same colour
						break
				for d in range(1, min(7-r,c)+1):
					if board[r+d][c-d] == " ":
						moves.append([[r,c], [r+d,c-d]])
					elif (board[r+d][c-d].islower() if side == "w" else board[r+d][c-d].isupper()):
						moves.append([[r,c], [r+d,c-d]])
						break
					else: #must be of same colour
						break
				for d in range(1, min(r,7-c)+1):
					if board[r-d][c+d] == " ":
						moves.append([[r,c], [r-d,c+d]])
					elif (board[r-d][c+d].islower() if side == "w" else board[r-d][c+d].isupper()):
						moves.append([[r,c], [r-d,c+d]])
						break
					else: #must be of same colour
						break
				for d in range(1, min(r,c)+1):
					if board[r-d][c-d] == " ":
						moves.append([[r,c], [r-d,c-d]])
					elif (board[r-d][c-d].islower() if side == "w" else board[r-d][c-d].isupper()):
						moves.append([[r,c], [r-d,c-d]])
						break
					else: #must be of same colour
						break				
			elif piece == "n":
				pnts = [p for p in [[r+1,c+2], [r+1,c-2], [r-1,c+2], [r-1,c-2], [r+2,c+1], [r-2,c+1], [r+2,c-1], [r-2,c-1]] if p[0] >= 0 and p[1] >= 0 and p[0] < 8 and p[1] < 8]
				for p in pnts:
					if board[p[0]][p[1]] == " ":
						moves.append([[r,c], p])
					elif (board[p[0]][p[1]].islower() if side == "w" else board[p[0]][p[1]].isupper()):
						moves.append([[r,c], p])
			elif piece == "k":
				pnts = [p for p in [[r+1,c], [r+1,c+1], [r, c+1], [r-1,c+1], [r-1,c], [r-1,c-1], [r,c-1], [r+1,c-1]] if p[0] >= 0 and p[1] >= 0 and p[0] < 8 and p[1] < 8]
				for p in pnts:
					if board[p[0]][p[1]] == " ":
						moves.append([[r,c], p])
					elif (board[p[0]][p[1]].islower() if side == "w" else board[p[0]][p[1]].isupper()):
						moves.append([[r,c], p])
	moves = [m for m in moves if not inCheck(makeMove(state, m), side)]
	
	return moves
					

def display(state):
	for row in reversed(state.split("/")[:8]):
		print(row)

def makeMove(state, move):
	parts = state.split("/")
	board = [list(r) for r in parts[:8]]
	others = parts[8:]

	board[move[1][0]][move[1][1]] = board[move[0][0]][move[0][1]]
	board[move[0][0]][move[0][1]] = " "

	return "/".join(["".join(r) for r in board] + others)
