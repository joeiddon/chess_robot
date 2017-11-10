import math, serial, time

ser = serial.Serial("/dev/ttyUSB0", 9600)

armGrabbed = False
armPos = [0, 20, 10]
armSpd = 0.02
armSteps = 70

def home():
    slideTo(0, 20, 10)

def repeat(p1, p2):
    place(p1, p2)
    place(p2, p1)

def place(p1, p2):
    slp = 0.8
    global armGrabbed
    home()
    slideTo(p1[0], p1[1], p1[2] + 8)
    slideTo(*p1)
    time.sleep(slp)
    armGrabbed = True
    slideTo(p1[0], p1[1], p1[2] + 8)
    home()
    slideTo(p2[0], p2[1], p2[2] + 8)
    slideTo(*p2)
    time.sleep(slp)
    armGrabbed = False
    time.sleep(slp)
    slideTo(p2[0], p2[1], p2[2] + 8)
    home()


def lerp(t, v0, v1):
    return v0 + t * (v1 - v0)

def lerp3d(i, p1, p2):
    return [lerp(i, p1[c], p2[c]) for c in range(3)]

def slideTo(x, y, z):
    global armPos
    for i in range(armSteps):
        moveTo(*lerp3d(i/armSteps, armPos, [x, y, z]))
        time.sleep(armSpd)
    armPos = [x, y, z]

def moveTo(x, y, z):
    frontServoOffset = -16
    backServoOffset = -8
    bottomServoOffset = -28
    gripperDistance = 4
    
    d = math.sqrt(x**2 + y**2) - gripperDistance

    frontServoPos = math.degrees(math.acos((d**2 + z**2) / (40*math.sqrt(d**2 + z**2))) + math.atan2(z, d))
    backServoPos = frontServoPos - 2 * math.degrees(math.atan2(z, d))
    bottomServoPos = math.degrees(math.atan2(x, y)) + 90

    #grabber, back servo, front servo, rotate, disconnected
    payload = [150 if armGrabbed else 121, 180 - int(backServoPos) + backServoOffset, 180 - int(frontServoPos)  + frontServoOffset, 180 - int(bottomServoPos) + bottomServoOffset, 10]

    ser.write(payload) 

home()
