#include <Servo.h>

/*
 Prerequisites:
  - all distances are in *millimeters*
*/

//constants
#define PI 3.141592

//pin definitions
#define STEPPER_ENABLE 2
#define STEPPER_DIR    3
#define STEPPER_STEP   4
#define ENDSTOP        5
#define SERVOS_ENABLE  6
#define SERVO_BACK     7
#define SERVO_FRNT     8
#define SERVO_GRABBER  9

//servo offsets (microseconds pulse widths)
#define SERVO_OFFSET_FRNT -230
#define SERVO_OFFSET_BACK -520

//grabber position in degrees <--------   TODO change to pulse widths !!!
#define SERVO_GRAB_POS_ON   2211
#define SERVO_GRAB_POS_OFF  2055

//relevant dimensions of arm
#define SEGMENT_LENGTH_1 200
#define SEGMENT_LENGTH_2 200
#define GRIPPER_HEIGHT   60  //missed this in the old version!
#define GRIPPER_LENGTH   40

//stepper direction macros
#define LEFT  1
#define RIGHT 0

//how far does the belt move per step?
#define STEP_DIST 0.15798
//delay in microseconds between each full step
#define STEP_SPEED 800
//full-half-quater-eigth--or-sixteenth-step
#define STEP_MODE  16

//servo home position:  d, z
#define SERVO_HOME    150, 50

/*
==Polulo A4988 Mode Selection==
  MS1 | MS2 | MS3 | Resolution
  0   | 0   | 0   | Full
  1   | 0   | 0   | Half
  0   | 1   | 0   | Quarter
  1   | 1   | 0   | Eighth
  1   | 1   | 1   | Sixteenth

==Joe's Serial Protocol==
  First byte is message type; last byte is terminator of val: 255.
  The number of enclosed bytes (NEBs) is dependent on the message type.
  First byte  | Message (type)  | NEBs | Enclosed bytes meaning
  0           | motor state     | 1    | (0/1 => disable/enable)
  1           | set grabber     | 1    | (0/1 => grab off/grab on
  2           | move position   | 3    | (x: [0¬34, y: [10¬41], z: [-5¬20]) //need to be 6 NEBs, because now in mm !!!!!!
  Will return a standard success code: 0/1 indicating success/failure, respectively.
*/

//initialise servo objects
Servo servo_back;
Servo servo_frnt;
Servo servo_grabber;

//globals
uint16_t x_pos;
uint8_t serial_buffer[4];
uint8_t *buf_pointer = &serial_buffer[0];

void setup() {
    //attach servo objects to their pins, setting end widths
    servo_back.attach(SERVO_BACK, 500, 2500);
    servo_frnt.attach(SERVO_FRNT, 500, 2500);
    servo_grabber.attach(SERVO_GRABBER, 500, 2500);
    //set modes of all digital pins
    pinMode(STEPPER_ENABLE, OUTPUT);
    pinMode(STEPPER_DIR,    OUTPUT);
    pinMode(STEPPER_STEP,   OUTPUT);
    pinMode(ENDSTOP,        INPUT);
    pinMode(SERVOS_ENABLE,  OUTPUT);
    //connect to our master
    Serial.begin(9600);

    //initial home (x_pos -> 0)
    go_home();
}

void loop() {
    /*
    if (Serial.available()){
        uint8_t b = Serial.read();
        if (s == 255){
            handle_message(); //remember to reply here
            buf_pointer = &serial_buffer[0];
        } else {
            if (buf_pointer < &serial_buffer[3]+1)
            serial_buffer[buf_pointer++] = b;
            else
            Serial.write(1); //message too long
        }
    }
    delay(1000);
    */
    move_to(300, SERVO_HOME);
    delay(200);
    move_to(150, SERVO_HOME);
    set_grabber(1);
    delay(1000);
    move_to(0, SERVO_HOME);
    set_grabber(0);
    delay(1000);
}

void handle_message(){
    switch (serial_buffer[0]){
        case 0: //set motors
            if (buf_pointer == &serial_buffer[2]){
                if (serial_buffer[1]) enable_motors();
                else disable_motors();
                Serial.write(0);
            } else {
                Serial.write(1); //message too long
            } break;
        case 1: //set grabber
              //TODO: write grabber function,
              //      -finish these types
        default:
            Serial.write(1); //unknown message type
    }
}

void move_to(int16_t x, int16_t y, int16_t z){
    //move to position (x,y,z)
    //x-axis is stepper
    //y-axis is horizontally perpendicular
    //z-axis is, therefore, up
    //right-haned co-ordinate system

    //enable motors, in case not already
    enable_motors();
    //servos
    move_servos(y, z);
    //step stepper to x postition - needs
    for (int i = 0; i < (x < x_pos ? x_pos - x : x - x_pos) / STEP_DIST * STEP_MODE; i++){
        step_stepper(x < x_pos ? LEFT : RIGHT);
        delayMicroseconds(STEP_SPEED / STEP_MODE);
    }
    x_pos = x;
}

void move_servos(int16_t d, int16_t z){
    //[modulised as used when homing too]
    float angles[2];
    get_servo_angles(d, z, angles);
    //write angles: offset, map to pulse widths whilst fixing direction
    servo_back.writeMicroseconds(2500 - angles[0] * (2000 / PI) + SERVO_OFFSET_BACK);
    servo_frnt.writeMicroseconds(2500 - angles[1] * (2000 / PI) + SERVO_OFFSET_FRNT);
}

void get_servo_angles(int16_t d, int16_t z, float angles_out[2]) {
    //d is horizontal distance of gripper from base
    //z is veritcal distance (height) of gripper from base
    //servo angles => angles_out = [back_angle, front_angle] in radians
    //see image in repo for calculations reference (meaning of letters)
    z += GRIPPER_HEIGHT; //need to go higher as gripper has height
    d -= GRIPPER_LENGTH; //don't need to go as far as gripper has length
    float Z = atan2(z, d);
    float A = acos((sq(SEGMENT_LENGTH_1) + sq((int32_t)d) + sq((int32_t)z) - sq(SEGMENT_LENGTH_2)) /
                   (2 * SEGMENT_LENGTH_1 * sqrt(sq((int32_t)d) + sq((int32_t)z))));
    float B = acos((sq(SEGMENT_LENGTH_2) + sq((int32_t)d) + sq((int32_t)z) - sq(SEGMENT_LENGTH_1)) /
                   (2 * SEGMENT_LENGTH_2 * sqrt(sq((int32_t)d) + sq((int32_t)z))));
    angles_out[0] = B - Z;
    angles_out[1] = A + Z;
}

void go_home(){
    enable_motors();
    move_servos(SERVO_HOME);
    set_grabber(0);
    while (!digitalRead(ENDSTOP)){
        step_stepper(LEFT);
        delayMicroseconds(STEP_SPEED / STEP_MODE);
    }
    x_pos = 0;
}

void step_stepper(uint8_t dir){
    digitalWrite(STEPPER_DIR, dir);
    digitalWrite(STEPPER_STEP, LOW);
    digitalWrite(STEPPER_STEP, HIGH);
}

void enable_motors(){
    if (!servo_back.attached())    servo_back.attach(SERVO_BACK);
    if (!servo_frnt.attached())    servo_frnt.attach(SERVO_FRNT);
    if (!servo_grabber.attached()) servo_grabber.attach(SERVO_GRABBER);
    digitalWrite(SERVOS_ENABLE,  HIGH);
    digitalWrite(STEPPER_ENABLE, LOW);
}

void disable_motors(){
    if (servo_back.attached())    servo_back.detach(SERVO_BACK);
    if (servo_frnt.attached())    servo_frnt.detach(SERVO_FRNT);
    if (servo_grabber.attached()) servo_grabber.detach(SERVO_GRABBER);
    digitalWrite(SERVOS_ENABLE,  LOW);
    digitalWrite(STEPPER_ENABLE, HIGH);
}

void go_offset_calibration(){
    servo_frnt.writeMicroseconds(1500 + SERVO_OFFSET_FRNT);
    servo_back.writeMicroseconds(2500 + SERVO_OFFSET_BACK);
}

void set_grabber(uint8_t state){
    //state is 1/0 indicating grab on/grab off
    servo_grabber.writeMicroseconds(state ? SERVO_GRAB_POS_ON : SERVO_GRAB_POS_OFF);
}
