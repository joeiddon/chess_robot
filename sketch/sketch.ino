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

//grabber position in pulse widths
#define SERVO_GRAB_POS_ON   2190
//
#define SERVO_GRAB_POS_OFF  1970

//relevant dimensions of arm
//GRIPPER_HEIGHT must be less than PIVOT_HEIGHT as using unsigned
#define SEGMENT_LENGTH_1 200
#define SEGMENT_LENGTH_2 200
#define GRIPPER_HEIGHT   50  //missed this in the old version!
#define GRIPPER_LENGTH   40
#define PIVOT_HEIGHT     60  //servo pivot height from ground

//stepper direction macros
#define LEFT  1
#define RIGHT 0

//how far does the belt move per step?
#define STEP_DIST 0.15798
//delay in microseconds between each full step
#define STEP_SPEED 1300
//full-half-quater-eigth--or-sixteenth-step
#define STEP_MODE  16

//servo home position:  d, z
#define SERVO_HOME    150, 50

//serial
#define BUF_LENGTH 7
#define INT_FROM_BYTES(A, B) (int16_t) ((A)<<8)+(B)

/*
==Polulo A4988 Mode Selection==
  MS1 | MS2 | MS3 | Resolution
  0   | 0   | 0   | Full
  1   | 0   | 0   | Half
  0   | 1   | 0   | Quarter
  1   | 1   | 0   | Eighth
  1   | 1   | 1   | Sixteenth

==Joe's Serial Protocol==
  First byte is message type; last byte is a full terminater byte (0xff).
  The number of enclosed bytes (NEBs) is dependent on the message type.
  First byte  | Message (type)  | NEBs | Enclosed bytes meaning
  0           | home            | 0    | N/A
  1           | motor state     | 1    | (0/1 => disable/enable)
  2           | set grabber     | 1    | (0/1 => grab off/grab on
  3           | move position   | 6    | (x: [0¬340, y: [100¬410], z: [0¬200])
                                          ^          ^             ^
                         these uint16s are sent *little-endian* (see INT_FROM_BYTES)
  Will return a standard success code: 0/1 indicating success/failure, respectively.

==Debugging Random Stepper Movements==
 -check STEP_SPEED
 -check motors are enabled (enable_motors called before x_pos set)
*/

//initialise servo objects
Servo servo_back;
Servo servo_frnt;
Servo servo_grabber;

//x current and target postions are steps (or sub-steps) from home.
//uint16_t big enough as 400 * STEP_DIST * 16 < 65536 (max)
uint16_t x_pos;
uint16_t x_target;

//timings for pulses (us == microseconds)
uint32_t last_pulse_us; //initialised in go_home();
uint32_t pulse_interval_us = STEP_SPEED / STEP_MODE;

//serial
uint8_t serial_buffer[BUF_LENGTH];
uint8_t buf_pointer = 0;

uint8_t counter = 0;

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
    //debug LED
    pinMode(13, OUTPUT);
    //connect to our master
    Serial.begin(9600);

    go_home();
}

void loop() {
    //no blocking functions allowed, this needs to zip
    //around as fast as possible for the stepper steps
    if (Serial.available()){
        uint8_t b = Serial.read();
        if (b == 0xff){
            handle_message(); //remember to reply here
            buf_pointer = 0;
        } else {
            if (buf_pointer < BUF_LENGTH) {
                serial_buffer[buf_pointer++] = b;
            } else {
                buf_pointer = 0;  //reset buf_pointer to start
                Serial.write(1); //message too long
            }
        }
    }
    //if not at stepper target and due to pulse, let's pulse
    if (x_pos != x_target && (micros() - last_pulse_us) >= pulse_interval_us){
        digitalWrite(13, HIGH);
        last_pulse_us += pulse_interval_us;
        step_stepper(x_pos < x_target ? RIGHT : LEFT);
        //if going home, we listen for endstop instead of blindly moving :)
        if (x_target == 0){ 
            if (digitalRead(ENDSTOP)){
                x_pos = 0;
           }
        } else {
            x_pos += x_pos < x_target ? 1 : -1;
       }
    } else {digitalWrite(13, LOW);}
}

void handle_message(){
    switch (serial_buffer[0]){
        case 0: //home
            if (buf_pointer == 1){
                go_home();
            } else {
                Serial.write(1);
            } break;
        case 1: //set motors
            if (buf_pointer == 2){
                if (serial_buffer[1]) enable_motors();
                else disable_motors();
                Serial.write(0);
            } else {
                Serial.write(1); //message too long
            } break;
        case 2: //set grabber
            if (buf_pointer == 2){
                set_grabber(serial_buffer[1]);
            } else {
                Serial.write(1);
            } break;
        case 3: //move to
            if (buf_pointer == 7){
                move_to(INT_FROM_BYTES(serial_buffer[1], serial_buffer[2]),
                        INT_FROM_BYTES(serial_buffer[3], serial_buffer[4]),
                        INT_FROM_BYTES(serial_buffer[5], serial_buffer[6]));
            } else {
                Serial.write(1);
            } break;
        default:
            Serial.write(1); //unknown message type
            break;
    }
}

void move_to(int16_t x, int16_t y, int16_t z){
    //move to position (x,y,z)
    //right-handed co-ordinate system, orientated:
    // - x-axis is stepper; measured from home
    // - y-axis is horizontally perpendicular towards the gripper; measured from servo pivots
    // - z-axis is, therefore, vertical; measured from the ground
    //enable motors, in case not already
    enable_motors();
    //move servos
    move_servos(y, z);
    //convert x coordinate in mm to steps, and set as target
    x_target = x / STEP_DIST * STEP_MODE;
    //need to pretend we just pulsed so timeings re-baised from here
    last_pulse_us = micros();
}

void move_servos(int16_t d, int16_t z){
    //[modulised as used when homing too]
    //moves the front and back servos to their correct angles
    //see get_servo_angles for meaning of parameters
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
    z -= PIVOT_HEIGHT;   //don't need to go as high as servo pivots already in air
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
    //mainly self-explanatory...
    enable_motors();
    move_servos(SERVO_HOME);
    set_grabber(0);
    x_pos = 1;    //tricks pulsing code into moving
    x_target = 0; //special case in pulsing code
    last_pulse_us = micros();
}

void step_stepper(uint8_t dir){
    digitalWrite(STEPPER_DIR, dir);
    digitalWrite(STEPPER_STEP, LOW);
    digitalWrite(STEPPER_STEP, HIGH);
}

void enable_motors(){
    digitalWrite(SERVOS_ENABLE,  HIGH);
    digitalWrite(STEPPER_ENABLE, LOW);
}

void disable_motors(){
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
