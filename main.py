import arm_lib, glob, time

#positions of chess board
X_SIDES = 130
Y_CLOSE = 140
Y_FAR   = 415
Z_PIECE =  10
Z_ABOVE =  105

#position to go to between moves
MOVE_POS = (int(arm_lib.LIMITS['x'][1]/2),
            int((Y_CLOSE + Y_FAR)/2),
            100)

#REST_POS = ...

#timing, in seconds
SETTLE_TIME = 1
GRAB_TIME   = 0.5

def get_serial_port():
    ser_ports = glob.glob('/dev/ttyUSB*')
    return ser_ports[0] if len(ser_ports) == 1 else \
           input(f'Which serial port from: {ser_ports}? ')

def get_piece_coordinate(x,y):
    '''Maps between piece coordinates [0-8,0-8] to arm coordinates in mm'''
    if not (0 <= x <= 8 and 0 <= y <= 8): raise ValueError('Piece coord out of range!')
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

arm = arm_lib.Arm(get_serial_port())
