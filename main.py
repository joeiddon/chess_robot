#built-in libraries
import glob, time, subprocess, warnings
#numpy
import numpy as np
#my libraries
import arm_lib, recognition

#turm off picamera warnings...
warnings.simplefilter('ignore')

#positions of chess board
#note sides are to centres of edge pieces
X_LEFT  =  14
X_RIGHT = 260 + X_LEFT
Y_CLOSE = 195
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
THOUGHT_TIME = 4

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
    for p in [[7,0],[7,7]]:
        print(*get_piece_coordinate(*p), MOVE_POS[2])
        arm.move_to(*get_piece_coordinate(*p), MOVE_POS[2])
        time.sleep(SETTLE_TIME)
        arm.block_till_reach_target()
        arm.move_to(*MOVE_POS)
        time.sleep(SETTLE_TIME)
        arm.block_till_reach_target()

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
            pass #not implementing for black takes as of now as shouldn't matter
    #update state
    if move[1][0] == 0 and state[move[0][0]][move[0][1]] == 'p':
        #if black pawn promote, handle acordingly
        state[move[1][0]][move[1][1]] = 'q'
    else:
        state[move[1][0]][move[1][1]] = state[move[0][0]][move[0][1]]
    state[move[0][0]][move[0][1]] = ' '

def get_user_move(last_whites, last_blacks):
    #user is assumed to play as black [for now]
    global white_pieces, black_pieces
    this_whites, this_blacks = recognition.recognise(0)
    #last condition required for when white takes
    move_from = last_blacks & ~this_blacks# & ~this_whites
    move_to   = this_blacks & ~last_blacks
    if np.sum(move_from) + np.sum(move_to) != 2:
        print('move from', move_from)
        print('move to', move_to)
        raise Exception('wrong number of pieces changed')
    else:
        white_pieces = this_whites
        black_pieces = this_blacks
    return [[b[0] for b in np.where(a)] for a in [move_from, move_to]]

def get_comp_move():
    #computer is assumed to play as white for now
    #calls the chess_ai C program with the state
    #to get the best move for the computer
    call = subprocess.run(['intelligence/chess_ai',
                           'white',
                           chr(THOUGHT_TIME),
                           ''.join(''.join(r) for r in state)],
                          stdout=subprocess.PIPE)
    parts = [int(i) for i in call.stdout.strip().split(b',')]
    return [parts[:2],parts[2:]]

state = [['R','N','B','Q','K','B','N','R'],
         ['P']*8, [' ']*8, [' ']*8, [' ']*8, [' ']*8, ['p']*8,
         ['r','n','b','q','k','b','n','r']]

white_pieces = np.array([[c.isupper() for c in r] for r in state], 'bool')
black_pieces = np.array([[c.islower() for c in r] for r in state], 'bool')

'''
print(get_comp_move())
display_state()
make_move(get_comp_move())
display_state()

raise SystemExit
'''
arm = arm_lib.Arm(get_serial_port())
try:
    arm.home()
except:
    pass #arduino "skips a beat" occasionally so pass silently
arm.block_till_reach_target()

calibrate()
arm.move_to(*REST_POS)
arm.block_till_reach_target()

display_state()
comp_move = get_comp_move()
print('computer plays: ', comp_move)
make_move(comp_move)
move_piece(comp_move[0][::-1], comp_move[1][::-1])
display_state()
arm.set_motors(0)

while 1:
    input('hit enter when you\'ve made a move...')
    print('recognising board...')
    try:
        user_move = get_user_move(white_pieces[:], black_pieces[:])
    except:
        print('recognition error')
        if input('show what I\'m seeing? y/n\n') == 'y': recognition.recognise(1)
        continue
    print('you played: ', user_move)
    make_move(user_move)
    display_state()
    comp_move = get_comp_move()
    comp_is_take = state[comp_move[1][0]][comp_move[1][1]] != ' '
    print('computer plays: ', comp_move)
    make_move(comp_move)
    display_state()
    #must transpose for move_piece and take_piece funcs
    if comp_is_take:
        take_piece(comp_move[1][::-1])
    move_piece(comp_move[0][::-1], comp_move[1][::-1])
    arm.move_to(*REST_POS)
    arm.block_till_reach_target()
    arm.set_motors(0)
