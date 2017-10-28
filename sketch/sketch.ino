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
  
  servo1.write(90);
  servo2.write(90);
  servo3.write(90);
  servo4.write(90);
  servo5.write(90);
}

// How many bytes have we already read this line?
byte curPos = 0;
// Store as we read
byte moveServo[5];

void loop(){
  if (Serial.available()){
    byte incoming = Serial.read();
    if(incoming == '\n'){
      servo1.write((int) moveServo[0]);
      servo2.write((int) moveServo[1]);
      servo3.write((int) moveServo[2]);
      servo4.write((int) moveServo[3]);
      servo5.write((int) moveServo[4]);
      curPos = 0;
    } else {
      moveServo[curPos] = incoming;
      curPos ++;
    }
  }
}
