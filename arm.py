import math, serial, time, glob #, atexit

#GENERAL
grabbed = 0
ser_port = '/dev/ttyUSB0'

#TIMINGS
deg_per_sec = 50 #300  #degrees per second
pulse_intervals = 20     #number of degrees to turn for each "slowing" pulse
settle_time = 1 #time for mechanical wobble to stabilise

#ROBOT ARM CONFIG
servo_grab_on   = 154
servo_grab_off  = 135
servo_grab_time = 0.1   #time to grab
arm_lengths = 20
#so length of gripper and distance between the base pivots
length_offset = 10
#servo offsets
servo_offset_front  = -16
servo_offset_back   = -8
servo_offset_bottom = -25

#COORDINATES
x_sides = 17
y_close = 14
y_far   = 44
z_piece = 1
z_above = 9
home_pos = (0,(y_far+y_close)/2, 15)

ser_ports = glob.glob('/dev/ttyUSB*')
if ser_port not in ser_ports:
    print('ERR', ser_port, 'is not available')
    if ser_ports:
        ser_port = ser_ports[0]
        input('press any key to connect to ' + ser_port + ' instead')
    else:
        print('no serial ports found, exiting')
        raise SystemExit

ser = serial.Serial(ser_port, 9600)


##################################################################################
# consider moving chess piece logic (as opposed to kinematics) to different file #
##################################################################################

def test_piece_movement():
    move_piece((0,0),(7,0))
    move_piece((7,0),(0,0))

def calibrate():
    home()
    for c in ((0,0), (7,0), (7,7), (0,7)):
        pc = piece_coordinate(*c)
        move_to(*pc, z_above)
        time.sleep(settle_time)
        move_to(*pc,(z_above+z_piece)/2)
        time.sleep(settle_time/2)
        move_to(*pc,z_piece)
        time.sleep(settle_time/2)
        set_grabber(1)
        time.sleep(settle_time/2)
        move_to(*pc,z_above)
        home()
        time.sleep(settle_time)
        set_grabber(0)

def piece_coordinate(x, y):
    if not (0 <= x < 8 and 0 <= y < 8): raise ValueError
    #returns the x,y coord in the robot's coordinate system from
    #the board's 8x8 sysyem ((0,0) in bottom left; y going away)
    return (-x_sides if not x else lerp(x/7,-x_sides,x_sides),
             y_close if not y else lerp(y/7,y_close,y_far))

def move_piece(c1, c2):
    c1, c2 = (piece_coordinate(*c) for c in (c1, c2))
    home()
    set_grabber(0)
    print('aligning with c1...')
    move_to(*c1, z_above)
    time.sleep(settle_time)
    print('poised\npositioning for grab...')
    move_to(*c1, z_piece)
    print('ready to grab\ngrabbing...')
    set_grabber(1)
    print('grabbed\nescaping...')
    move_to(*c1, z_above)
    print('escaped\ngoing home...')
    home()
    print('home\naligning with c2...')
    move_to(*c2, z_above)
    time.sleep(settle_time)
    print('poised\npositioning for release...')
    move_to(*c2, z_piece)
    print('ready to release\nreleasing...')
    set_grabber(0)
    print('released\nescaping...')
    move_to(*c2, z_above)
    print('escpaed\ngoing home...')
    home()
    print('move complete')

def lerp(i, x1, x2):
    return x1 + i * (x2-x1)

def lerp3d(i, c1, c2):
    return [lerp(i, c1[j], c2[j]) for j in range(3)]

def dist3d(c1, c2):
    return ((c2[0]-c1[0])**2+(c2[1]-c1[1])**2+(c2[2]-c1[2])**2)**0.5

def set_grabber(state):
    global grabbed
    ser.write([servo_grab_on if state else servo_grab_off, 10])
    time.sleep(servo_grab_time)
    grabbed = state

def get_servo_angles(x, y, z):
    try:
        d = math.sqrt(x**2+y**2) - length_offset
        A = math.acos((d**2+z**2) / (2*arm_lengths*(d**2+z**2)**0.5))
        Z = math.atan2(z,d)
        R = math.atan2(x,y)

        front  = math.degrees(A+Z)
        back   = math.degrees(A-Z)
        bottom = math.degrees(R)+90
    except ValueError:
        print('TRIG_ERR: {},{},{}'.format(x,y,z))
        return 0
    return (front,back,bottom)

def move_to(x, y, z):
    global servo_cur_front, servo_cur_back, servo_cur_bottom

    angles = get_servo_angles(x,y,z)
    if not angles:
        print('err again')
        return

    servo_target_front, servo_target_back, servo_target_bottom = angles

    servo_change_front  = servo_target_front  - servo_cur_front
    servo_change_back   = servo_target_back   - servo_cur_back
    servo_change_bottom = servo_target_bottom - servo_cur_bottom

    max_change = max(map(abs, [servo_change_front, servo_change_back, servo_change_bottom]))

    write_servos(*lerp3d(0.5,
                        (servo_cur_back, servo_cur_front, servo_cur_bottom),
                        (servo_target_back, servo_target_front, servo_target_bottom)))
    time.sleep(max_change/deg_per_sec/2)
    write_servos(servo_target_back, servo_target_front, servo_target_bottom)
    time.sleep(max_change/deg_per_sec/2)

    servo_cur_front  = servo_target_front
    servo_cur_back   = servo_target_back
    servo_cur_bottom = servo_target_bottom

def write_servos(back, front, bottom):
    try:
        #grabber, back servo, front servo, rotate, end signal
        payload = [servo_grab_on if grabbed else servo_grab_off,
                   180 - int(back)   + servo_offset_back,
                   180 - int(front)  + servo_offset_front,
                   180 - int(bottom) + servo_offset_bottom,
                   10]

        ser.write(payload)
    except ValueError:
        print('SERVO_ERR: {},{},{}'.format(back,front,bottom))
        return 1
    return 0

def home():
    move_to(*home_pos)

#SERVO HOME ANGLES
servo_cur_front, servo_cur_back, servo_cur_bottom = get_servo_angles(*home_pos)

if __name__ == '__main__':
    home()

#atexit.register(home)
