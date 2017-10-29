import math, serial, time

ser = serial.Serial("/dev/ttyUSB0", 9600)

armPos = [0, 20, 10]
armSpd = 0.1

def home():
    moveTo(0, 20, 10)

def cube(xRange, yRange, zRange, slp):
    for z in zRange:
        for y in yRange:
            for x in xRange:
                print("moving to", x, y, z)
                moveTo(x, y, z)
                time.sleep(slp)

def lerp(t, v0, v1):
    return v0 + t * (v1 - v0)

def lerp3d(i, p1, p2):
    return [lerp(i, p1[c], p2[c]) for c in range(3)]

def slideTo(x, y, z):
    global armPos
    for i in range(10):
        moveTo(*lerp3d(i/10, armPos, [x, y, z]))
        time.sleep(armSpd)
    armPos = [x, y, z]

def moveTo(x, y, z):
    frontServoOffset = -23
    backServoOffset = -8
    bottomServoOffset = -28
    
    d = math.sqrt(x**2 + y**2)

    frontServoPos = math.degrees(math.acos((d**2 + z**2) / (40*math.sqrt(d**2 + z**2))) + math.atan2(z, d))
    backServoPos = frontServoPos - 2 * math.degrees(math.atan2(z, d))
    bottomServoPos = math.degrees(math.atan2(x, y)) + 90

    #grabber, back servo, front servo, rotate, disconnected
    payload = [121, 180 - int(backServoPos) + backServoOffset, 180 - int(frontServoPos)  + frontServoOffset, 180 - int(bottomServoPos) + bottomServoOffset, 10]

    ser.write(payload) 

home()
