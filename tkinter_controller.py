from tkinter import *
import time, serial, glob

#NOTE: this code is *not* very readable; it was
#      just a quick hack to test the positioning,
#      should handle arduino responses, but instead
#      just prints them
#
#   if want to run in background:
#   echo '/dev/ttyUSB0' | python tkinter_controller.py > /dev/null 2>&1 &

ser_ports = glob.glob('/dev/ttyUSB*')
print(f'possile serial directories: {ser_ports}')
s = serial.Serial(input('serial dir: ') if len(ser_ports) == 0 or len(ser_ports) > 1 else ser_ports[0], 115200)

root = Tk()
root.title('Robo Arm Controller')

x_max = 340
y_min = 100
y_max = 450
z_min = 0
z_max = 200

x = 0
y = 150

def move(*args):
    z = z_scale.get()
    print(f'move to {{x, y, z}}: [{x}, {y}, {z}]')
    s.write([3,*bytes_from_int(x),*bytes_from_int(y),*bytes_from_int(z),0xff])

def mouse_press(evt):
    global x,y
    x = 0 if evt.x < 0 else x_max if evt.x > x_max else evt.x
    y = y_max - (0 if evt.y < 0 else y_max-y_min if evt.y > y_max-y_min else evt.y)
    move()

def home():
    global x
    s.write([0,0xff])
    x = 0

def set_motor_state(state):
    global motors_enabled
    s.write([1,state,0xff])
    motors_enabled = state

border_width = 2
font = ('Courrier', 9)
#root.columnconfigure(5,minsize=(x_max+2*border_width)/4)
canvas = Canvas(root,height=y_max-y_min,width=x_max,highlightthickness=0,relief='sunken',bd=border_width)
canvas.bind('<Button-1>',mouse_press)
canvas.grid(row=1,column=1,columnspan=4,sticky='wens')

z_scale = Scale(root,from_=z_max,to=z_min,length=200,tickinterval=20,orient=VERTICAL, command=move)
z_scale.set(100)
z_scale.grid(row=1,column=5,sticky='wens')

home_but = Button(root,font=font,text='home',command=home)
home_but.grid(row=2,column=1,sticky='wens')

motors_enable_but = Button(root,font=font,text='enable mot.',command=lambda: set_motor_state(1))
motors_enable_but.grid(row=2,column=2,sticky='wens')
motors_disable_but = Button(root,font=font,text='disable mot.',command=lambda: set_motor_state(0))
motors_disable_but.grid(row=2,column=3,sticky='wens')

grabber_enable_but = Button(root,font=font,text='grab on',command=lambda: s.write([2,1,0xff]))
grabber_enable_but.grid(row=2,column=4,sticky='wens')
grabber_disable_but = Button(root,font=font,text='grab off',command=lambda: s.write([2,0,0xff]))
grabber_disable_but.grid(row=2,column=5,sticky='wens')

cross_size = 15

def bytes_from_int(i):
    return [i >> 8,i & 0xff]

while True:
    canvas.delete('all')
    canvas.create_line(x-cross_size,y_max-y-cross_size,x+cross_size,y_max-y+cross_size,fill='#f00',width=4)
    canvas.create_line(x+cross_size,y_max-y-cross_size,x-cross_size,y_max-y+cross_size,fill='#f00',width=4)

    if s.in_waiting:
        print(list(s.read(s.in_waiting)))

    root.update()
    time.sleep(0.05)
