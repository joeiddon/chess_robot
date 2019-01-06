#built-in libraries
import glob, time, subprocess
#numpy
import numpy as np
#my libraries
import arm_lib, recognition

#positions of chess board
X_SIDES = 130
Y_CLOSE = 178
Y_FAR   = 425
Z_PIECE =  33
Z_ABOVE = 105

#position to go to between moves
MOVE_POS = (int(arm_lib.LIMITS['x'][1]/2),
            int((Y_CLOSE + Y_FAR)/2),
            100)

REST_POS = (arm_lib.LIMITS['x'][1]-5, *MOVE_POS[1:])

#timing, in seconds
SETTLE_TIME = 0.9
GRAB_TIME   = 0.5

#how long does the computer have to think (in seconds)
THOUGHT_TIME = 1

def get_serial_port():
    ser_ports = glob.glob('/dev/ttyUSB*')
    return ser_ports[0] if len(ser_ports) == 1 else \
           input('Which serial port from:' + str(ser_ports) +'? ')

def get_piece_coordinate(x,y):
    '''Maps between piece coordinates [0-8,0-8] to arm coordinates in mm'''
    if not (0 <= x < 8 and 0 <= y < 8): raise ValueError('Piece coord out of range!')
    return (int(arm_lib.LIMITS['x'][1] / 2- X_SIDES + x/7 * 2 * X_SIDES),
            int(Y_CLOSE + y/7 * (Y_FAR - Y_CLOSE)))

def move_piece(c1, c2):
    c1, c2 = (get_piece_coordinate(*c) for c in (c1, c2))
    arm.set_grabber(0)
    arm.move_to(*MOVE_POS)
    arm.block_till_reach_target()
    arm.move_to(c1[0], *MOVE_POS[1:])
    arm.block_till_reach_target()
    arm.move_to(*c1, Z_ABOVE)
    time.sleep(SETTLE_TIME)
    arm.move_to(*c1, Z_PIECE)
    time.sleep(SETTLE_TIME)
    arm.set_grabber(1)
    time.sleep(GRAB_TIME)
    arm.move_to(*c1, Z_ABOVE);
    time.sleep(SETTLE_TIME)
    arm.move_to(c1[0], *MOVE_POS[1:])
    time.sleep(SETTLE_TIME)
    arm.move_to(c2[0], *MOVE_POS[1:])
    arm.block_till_reach_target()
    arm.move_to(*c2, Z_ABOVE);
    time.sleep(SETTLE_TIME)
    arm.move_to(*c2, Z_PIECE);
    time.sleep(SETTLE_TIME)
    arm.set_grabber(0)
    time.sleep(GRAB_TIME)
    arm.move_to(*c2, Z_ABOVE);
    time.sleep(SETTLE_TIME)
    arm.move_to(c2[0], *MOVE_POS[1:])
    time.sleep(SETTLE_TIME)
    arm.move_to(*MOVE_POS)

def display_state():
    print('-'*10)
    for i,r in enumerate(state[::-1]):
        print(7-i,''.join(r))
    print('  '+''.join(str(i) for i in range(8))+'\n'+'-'*10)

def make_move(move):
    #makes a move to the state
    state[move[1][0]][move[1][1]] = state[move[0][0]][move[0][1]]
    state[move[0][0]][move[0][1]] = ' '

def get_user_move(last_whites, last_blacks):
    #user is assumed to play as black [for now]
    #global white_pieces, black_pieces
    white_pieces, black_pieces = recognition.recognise(0)
    move_from = last_blacks & ~black_pieces
    move_to   = black_pieces & ~last_blacks
    if np.sum(move_from) + np.sum(move_to) != 2:
        raise Exception('wrong number of pieces changes')
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
time.sleep(2)
arm.home()
arm.block_till_reach_target()

subprocess.run(['intelligence/chess_ai', 'white', chr(1), ''.join(''.join(r) for r in state)],stdout=subprocess.PIPE).stdout.strip()
while 1:
    display_state()
    move = get_user_move(white_pieces[:], black_pieces[:])
    print(move)
    #rember to transpose for move_piece function
    move_piece(move[1][::-1], move[0][::-1])
    arm.move_to(*REST_POS)
    input()

display_state()
