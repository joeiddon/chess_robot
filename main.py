#built-in libraries
import glob, time, subprocess, warnings, ast
#numpy
import numpy as np
#my libraries
import arm_lib, recognition

print('libraries loaded successfully')

#turm off picamera warnings...
warnings.simplefilter('ignore')

#positions of chess board
#note sides are to centres of edge pieces
X_LEFT  =  14
X_RIGHT = 260 + X_LEFT
Y_CLOSE = 190
Y_FAR   = 431
Z_PIECE =  35
Z_ABOVE = 100

#position to go to during moves
MOVE_POS = (int((X_LEFT + X_RIGHT)/2),
            int((Y_CLOSE + Y_FAR)/2),
            130)
#position to go to between moves
REST_POS = (arm_lib.LIMITS['x'][1], *MOVE_POS[1:])

#position of piece pot
POT_Y_DIST = 400
POT_Z_ABOVE = 170
POT_Z_IN = 80

#timing, in seconds
SETTLE_TIME = 0.4 #time for servos to move and settle
GRAB_TIME   = 0.4 #time for grabber to open/close

#how long does the computer have to think (in seconds)
THOUGHT_TIME = 3

def get_serial_port():
    ser_ports = glob.glob('/dev/ttyUSB*')
    return ser_ports[0] if len(ser_ports) == 1 else \
           input('Which serial port from:' + str(ser_ports) +'? ')

def get_piece_coordinate(x,y):
    '''maps between piece coordinates [0-8,0-8] to arm coordinates in mm'''
    if not (0 <= x < 8 and 0 <= y < 8): raise ValueError('Piece coord out of range!')
    return (int(X_LEFT  + x/7 * (X_RIGHT - X_LEFT)),
            int(Y_CLOSE + y/7 * (Y_FAR - Y_CLOSE)))

def calibrate():
    '''servos need a bit of 'warming up', so a little calibration routine'''
    #should have added more move calls here, but only hacky for now
    for p in [[0,0],[0,7],[7,7],[7,0]]:
        #print(*get_piece_coordinate(*p), MOVE_POS[2])
        arm.move_to(*get_piece_coordinate(*p), MOVE_POS[2])
        time.sleep(SETTLE_TIME)
        arm.block_till_reach_target()
        arm.move_to(*MOVE_POS)
        time.sleep(SETTLE_TIME)
        arm.block_till_reach_target()
        time.sleep(0.5)

def tune_arm():
    '''moves piece on first column sequentially up, then down, the rows'''
    for r in range(7):
        move_piece((0,r),(0,r+1))
    for r in range(7,0,-1):
        move_piece((0,r),(0,r-1))

def move_piece(c1, c2):
    '''moves piece from c1 -> c2, note coords in x,y NOT r,c format'''
    c1, c2 = (get_piece_coordinate(*c) for c in (c1, c2))
    arm.set_grabber(0)                  #open grabber
    arm.move_to(c1[0], *MOVE_POS[1:])   #move to column of it
    arm.block_till_reach_target()
    arm.move_to(*c1, Z_ABOVE)           #move above it
    time.sleep(SETTLE_TIME)
    arm.move_to(*c1, Z_PIECE)           #move to it
    time.sleep(SETTLE_TIME)
    arm.set_grabber(1)                  #grab it
    time.sleep(GRAB_TIME)
    arm.move_to(*c1, Z_ABOVE)           #move above it
    time.sleep(SETTLE_TIME)
    arm.move_to(c1[0], *MOVE_POS[1:])   #move to centre on it's column
    time.sleep(SETTLE_TIME)
    arm.move_to(c2[0], *MOVE_POS[1:])   #move to centre on end's column
    arm.block_till_reach_target()
    arm.move_to(*c2, Z_ABOVE)           #move above end pos
    time.sleep(SETTLE_TIME)
    arm.move_to(*c2, Z_PIECE)           #move to end pos
    time.sleep(SETTLE_TIME)
    arm.set_grabber(0)                  #release at end pos
    time.sleep(GRAB_TIME)
    arm.move_to(*c2, Z_ABOVE);          #move above end pos
    time.sleep(SETTLE_TIME)
    arm.move_to(c2[0], *MOVE_POS[1:])   #move to centre on end's column
    time.sleep(SETTLE_TIME)
    arm.move_to(*REST_POS)              #move to rest pos
    arm.block_till_reach_target()

def take_piece(c):
    '''takes piece at coordinate c, note in x.y NOT r,c format'''
    c = get_piece_coordinate(*c)
    arm.set_grabber(0)                  #open grabber
    arm.move_to(c[0], *MOVE_POS[1:])    #move to column above piece
    arm.block_till_reach_target()
    arm.move_to(*c, Z_ABOVE)            #move above it
    time.sleep(SETTLE_TIME)
    arm.move_to(*c, Z_PIECE)            #move to it
    time.sleep(SETTLE_TIME)
    arm.set_grabber(1)                  #grab their piece (take it)
    time.sleep(GRAB_TIME)
    arm.move_to(*c, Z_ABOVE)            #move above it
    time.sleep(SETTLE_TIME)
    arm.move_to(c[0], *MOVE_POS[1:])    #move to centre on it's column
    time.sleep(SETTLE_TIME)
    arm.move_to(*REST_POS)              #move to rest pos
    arm.block_till_reach_target()
    arm.move_to(REST_POS[0],            #move above pot
                POT_Y_DIST,
                POT_Z_ABOVE)
    time.sleep(SETTLE_TIME)
    arm.move_to(REST_POS[0],            #move in pot
                POT_Y_DIST,
                POT_Z_IN)
    time.sleep(SETTLE_TIME)
    arm.set_grabber(0)                  #release piece
    time.sleep(GRAB_TIME)
    arm.move_to(REST_POS[0],            #move above pot
                POT_Y_DIST,
                POT_Z_ABOVE)
    time.sleep(SETTLE_TIME)
    arm.move_to(*REST_POS)              #move to rest pos
    time.sleep(SETTLE_TIME)

def display_state():
    print('-'*10)
    for i,r in enumerate(state[::-1]):
        print(7-i,''.join(r))
    print('  '+''.join(str(i) for i in range(8))+'\n'+'-'*10)

def make_move(move):
    '''makes a move to the state and maybe black_pieces/white_pieces'''
    #only need to update recognition board state (black_pieces and white_pieces)
    #if it is a *take*
    global black_pieces
    if state[move[1][0]][move[1][1]]:
        if state[move[0][0]][move[0][1]].isupper(): #if moving a white piece
            black_pieces[move[1][0]][move[1][1]] = 0
        else:
            #not implementing for black takes as of now as we don't ever recognise() for white
            pass
    #update state
    #special case: pawn promotes
    if move[1][0] in (0,7) and state[move[0][0]][move[0][1]].lower() == 'p':
        state[move[1][0]][move[1][1]] = 'q' if state[move[0][0]][move[0][1]] == 'p' else 'Q'
    else:
        state[move[1][0]][move[1][1]] = state[move[0][0]][move[0][1]]
    state[move[0][0]][move[0][1]] = ' '

def get_user_move():
    #user is assumed to play as black [for now]
    global black_pieces
    input('hit enter when you have made a move...')
    print('recognising board...')
    _, new_blacks = recognition.recognise(0)
    blacks_gone = black_pieces & ~new_blacks
    blacks_new  = new_blacks   & ~black_pieces
    if np.sum(blacks_gone) + np.sum(blacks_new) != 2:
        print('board recognition error')
        if input('show what I\'m seeing? y/n\n') == 'y': recognition.recognise(1)
        return get_user_move() #try again
    move = [[b[0] for b in np.where(a)] for a in [blacks_gone, blacks_new]]
    if move not in black_info['moves']:
        print('illegal move\nyou played:', move, 'valid moves are:', white_info['moves'])
        return get_user_move() #try again
    black_pieces = new_blacks
    return move

def get_comp_move():
    #computer is assumed to play as white for now
    #calls the chess_ai C program with the state
    #to get the best move for the computer
    call = subprocess.run(['intelligence/chess_ai',
                           ''.join(''.join(r) for r in state),
                           'white',
                           'move',
                           str(THOUGHT_TIME)],
                          stdout=subprocess.PIPE)
    return ast.literal_eval(call.stdout.decode('utf-8').strip())

def get_side_info(side):
    #returns dict of if side is checkmated and their valid moves
    #also updates global state evaluation
    global evaluation
    call = subprocess.run(['intelligence/chess_ai',
                           ''.join(''.join(r) for r in state),
                           side,
                           'eval'],
                          stdout=subprocess.PIPE)
    is_checkmated, evaluation, moves = call.stdout.decode('utf-8').strip().split()
    return {'is_checkmated': int(is_checkmated),
            'moves': ast.literal_eval(moves)}

state = [['R','N','B','Q','K','B','N','R'],
         ['P']*8, [' ']*8, [' ']*8, [' ']*8, [' ']*8, ['p']*8,
         ['r','n','b','q','k','b','n','r']]

white_pieces = np.array([[c.isupper() for c in r] for r in state], 'bool')
black_pieces = np.array([[c.islower() for c in r] for r in state], 'bool')

arm = arm_lib.Arm(get_serial_port())

try:
    print('homing arm')
    try:
        arm.home()
    except: pass #arduino "skips a beat" occasionally so pass silently
    arm.block_till_reach_target()
    print('arm homed')
    if input('you want to calibrate? y/n\n') == 'y': calibrate()
    arm.move_to(*REST_POS)
    arm.block_till_reach_target()
    arm.set_motors(0)

    white_info = get_side_info('white')
    black_info = get_side_info('black')

    while 1:
        print('thinking of a move; will take ', THOUGHT_TIME, 'seconds')
        comp_move = get_comp_move()
        comp_is_take = state[comp_move[1][0]][comp_move[1][1]] != ' '
        print('computer plays: ', comp_move)
        make_move(comp_move)
        display_state()
        black_info = get_side_info('black')
        if black_info['is_checkmated']:
            print('black checkmated')
            break
        '''
        #must transpose for move_piece and take_piece funcs
        if comp_is_take:
            take_piece(comp_move[1][::-1])
        move_piece(comp_move[0][::-1], comp_move[1][::-1])
        arm.move_to(*REST_POS)
        arm.block_till_reach_target()
        arm.set_motors(0)
        '''

        user_move = get_user_move()
        print('you played: ', user_move)
        make_move(user_move)
        display_state()
        white_info = get_side_info('white')
        if white_info['is_checkmated']:
            print('white checkmated!')
            break
except:
    arm.home()
    arm.block_till_reach_target()
    arm.set_motors(0)
    raise ##re-raise the exception
