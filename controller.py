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
p=arm.home_pos #current position

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
        p = tuple(p[i] + d[c][i] for i in range(3))
        arm.move_to(*p)
    elif c=='e':
        g = not g
        arm.set_grabber(g)
    elif c=='h':
        arm.home()
        p = arm.home_pos
    elif c=='c':
        print('chck')
        arm.home()
        arm.move_to(*p)
    elif c=='q':
        arm.home()
        print()
        break
    print(''.join(str(i).rjust(5) for i in p)+' '*10,end='\r')
