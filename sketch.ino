#include <Servo.h>

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(13, OUTPUT);

  servo1.attach(2);
  servo2.attach(3);
  servo3.attach(4);
  servo4.attach(5);
  servo5.attach(6);
  grip(false);
  servo1.write(80);
  servo2.write(80);
  servo3.write(20);
  servo4.write(90);
  servo5.write(170);
}

// How many bytes have we already read this line?
byte curPos = 0;
// Store as we read
byte moveServo[4];

void loop(){
  if (Serial.available()){
    byte incoming = Serial.read();
    if(incoming == '\n'){
      servo2.write((int)moveServo[0]);
      servo3.write((int)moveServo[1]);
      servo4.write((int)moveServo[2]);
      servo5.write((int)moveServo[3]);
      currentPos = 0;
    } else {
      moveServo[currentPos] = incoming;
      currentPos ++;
    }
  }
}
