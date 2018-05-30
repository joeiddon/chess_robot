import tty, sys, termios, arm

print('''Controls:
w | foward
a | left
s | backward
d | right
z | up
e | grab
x | down
h | home
c | check
q | quit''')

s=2 #step
g=0 #grabber state

def get_ch():
    fd = sys.stdin.fileno()
    o_set = termios.tcgetattr(fd)
    try:
        tty.setraw(sys.stdin.fileno())
        ch = sys.stdin.read(1)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, o_set)
    return ch

print('\n    x    y    z')
while 1:
    c = get_ch()
    #print('\r|'+c+'|',end='')
    d = {'w': [0,s,0],
         'a': [-s,0,0],
         's': [0,-s,0],
         'd': [s,0,0],
         'z': [0,0,s],
         'x': [0,0,-s]}
    if c in d:
        arm.slide_to(arm.cur_pos[0] + d[c][0],
                     arm.cur_pos[1] + d[c][1],
                     arm.cur_pos[2] + d[c][2])
    elif c=='e':
        g = not g
        arm.set_grabber(g)
    elif c=='h':
        arm.home()
    elif c=='c':
        p = arm.cur_pos
        arm.home()
        arm.slide_to(*p)
    elif c=='q':
        arm.home()
        print()
        break
    print(''.join(str(i).rjust(5) for i in arm.cur_pos)+' '*10,end='\r')
