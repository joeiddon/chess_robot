from tkinter import *
import math, time, serial

ser = serial.Serial("/dev/ttyUSB0", 9600)

root = Tk()
root.title("Robo Arm Controller")

canvas = Canvas(root, height=500, width=500, highlightthickness=0,relief="sunken", bd=1)
canvas.grid(rowspan=6,column=1,sticky="wens",padx = 10, pady = 10)

activeColour = "#fff"

Label(root, text="Servo 2").grid(row=0,column=0)
servo2 = Scale(root, from_=180, to=0, width=20, length=150, tickinterval=30, activebackground=activeColour)
servo2.set(90);
servo2.grid(row=1,column=0,sticky="wens")

Label(root, text="Servo 3").grid(row=2,column=0)
servo3 = Scale(root, from_=180, to=0, width=20, length=150, tickinterval=30, activebackground=activeColour)
servo3.set(90);
servo3.grid(row=3,column=0,sticky="wens")

Label(root, text="Servo 4").grid(row=4,column=0)
servo4 = Scale(root, from_=180, to=0, width=20, length=150, tickinterval=30, activebackground=activeColour)
servo4.set(90);
servo4.grid(row=5,column=0,sticky="wens")

Label(root, text="Servo 5").grid(row=6,columnspan=2)
servo5 = Scale(root, from_=0, to=180, width=20, length=200, tickinterval=10, orient=HORIZONTAL, activebackground=activeColour)
servo5.set(90);
servo5.grid(row=7,columnspan=2,sticky="wens")

armLength1 = 144
armLength2 = 144
gripperLength = 120
s4x = 150
s4y = 450


while True:
  canvas.delete("all")
  
  s3x = s4x + math.cos(math.radians(servo4.get())) * armLength1
  s3y = s4y - math.sin(math.radians(servo4.get())) * armLength1
  s2x = s3x + math.cos(math.radians(servo4.get() - (servo3.get() - 20))) * armLength2
  s2y = s3y - math.sin(math.radians(servo4.get() - (servo3.get() - 20))) * armLength2
  gx =  s2x + math.cos(math.radians(servo4.get() - (servo3.get() - 20) + (servo2.get() - 80))) * gripperLength
  gy =  s2y - math.sin(math.radians(servo4.get() - (servo3.get() - 20) + (servo2.get() - 80))) * gripperLength
  ax = s4x + math.cos(math.radians(180-servo5.get())) * (abs(90-servo5.get()) * 0.3 + 40)
  ay = s4y + math.sin(math.radians(180-servo5.get())) * (abs(90-servo5.get()) * 0.3 + 40)
  
  colour = "#999"
  
  canvas.create_line(s4x,s4y,s3x,s3y,fill=colour, width=4, arrow="last")
  canvas.create_line(s3x,s3y,s2x,s2y,fill=colour, width=4, arrow="last")
  canvas.create_line(s2x,s2y,gx , gy, fill=colour, width=4, arrow="last")
  canvas.create_line(s4x,s4y,ax , ay, fill="#777", width=4, arrow="last")
  
  #print(chr(servo2.get())+chr(servo3.get())+chr(servo4.get())+chr(servo5.get())+'\n')
  #ser.write(bytes(chr(servo2.get())+chr(servo3.get())+chr(servo4.get())+chr(servo5.get())+'\n','utf8'))
  
  ser.write([servo2.get(),servo3.get(),servo4.get(),servo5.get(),10])  
  time.sleep(0.05)
  
  root.update()
