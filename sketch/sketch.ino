#include <Servo.h>

/*
 Prerequisites:
  - all distances are in *millimeters*

  TODO:
    - right better explanation for program here
    - add checks to move_to assert the inputs are valid
 
==Polulo A4988 Mode Selection==
  MS1 | MS2 | MS3 | Resolution
  0   | 0   | 0   | Full
  1   | 0   | 0   | Half
  0   | 1   | 0   | Quarter
  1   | 1   | 0   | Eighth
  1   | 1   | 1   | Sixteenth

==Joe's Serial Protocol==
  Runs at a baud rate of 115200 pulses per second.
  The number of bytes in the message is sent first.
  The next byte represents the message type.
  The following number of enclosed bytes (NEBs) is dependent on the message type.
  First byte  | Message (type)  | NEBs | Enclosed bytes meaning
  0           | home            | 0    | N/A
  1           | motor state     | 1    | (0/1 => disable/enable)
  2           | set grabber     | 1    | (0/1 => grab off/grab on
  3           | move position   | 6    | (x: [0¬340, y: [100¬450], z: [0¬200])
                                          ^          ^             ^
       these uint16s (actually casted to signed) are sent *little-endian* (see INT_FROM_BYTES)

  Arduino will reply with a one byte code after every message.
  Value       | Meaning
  0           | Task completed as expected
  1           | x_pos has reached x_target - useful for timings
  2           | Invalid request structure (wrong NEBs)
  3           | Task failed due to invalid input ranges
  4           | Unkown request type

==Debugging Random Stepper Movements==
 -check STEP_SPEED
 -check motors are enabled (enable_motors called before x_pos set)

*/

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
#define SERVO_OFFSET_FRNT -240
#define SERVO_OFFSET_BACK -640

//grabber position in pulse widths
#define SERVO_GRAB_POS_ON   2150
#define SERVO_GRAB_POS_OFF  1940

//relevant dimensions of arm
//GRIPPER_HEIGHT must be less than PIVOT_HEIGHT as using unsigned
#define SEGMENT_LENGTH_1 200
#define SEGMENT_LENGTH_2 200
#define GRIPPER_HEIGHT   50  //missed this in the old version!
#define GRIPPER_LENGTH   35
#define PIVOT_HEIGHT     60  //servo pivot height from ground

//stepper direction macros
#define LEFT  1
#define RIGHT 0
//full-half-quater-eigth-or-sixteenth-step
#define STEP_MODE 16
//how far does the belt move per step?
#define STEP_DIST 0.15798
//speeds are delays in microseconds between each full step
#define MAX_STEP_SPEED 1000
#define MIN_STEP_SPEED 2800  //<<< keep this low, accel_pulse_us array only bytes
//acceleration and deceleration will take this many millimeters
#define ACCEL_AND_DECELERATION_DIST  15

//number of sub-steps for acceleration and deceleration
#define NUM_ACCEL_OR_DECEL_STEPS (uint16_t) (ACCEL_AND_DECELERATION_DIST / STEP_DIST * STEP_MODE)

//predefined arm positions
//servo home position:  d,  z
#define SERVO_HOME    180,100
#define X_IN_RANGE(x)   0<=(x) && (x)<=340
#define Y_IN_RANGE(y) 100<=(y) && (y)<=450
#define Z_IN_RANGE(z)   0<=(z) && (z)<=200

//serial
#define BUF_LENGTH 7
#define INT_FROM_BYTES(A, B) (int16_t) ((A)<<8)+(B)
#define CODE_NORMAL           0
#define CODE_X_TARGET_REACHED 1
#define CODE_INVALID_NEBS     2
#define CODE_INVALID_INPUT    3
#define CODE_INVALID_TYPE     4

//initialise servo objects
Servo servo_back;
Servo servo_frnt;
Servo servo_grabber;

//x current and target postions are steps (or sub-steps) from home.
//uint16_t big enough as max x / STEP_DIST * 16 < 65536 (max)
uint16_t x_pos;
uint16_t x_target;

//set to 0 every time we move so we know when to stop accelerating
uint16_t pulses_since_accel_start;
//array to store accel/decel pulse lengths as they take too long
//to calculate on the fly, so computed in setup() and stored
uint8_t  accel_pulse_us[NUM_ACCEL_OR_DECEL_STEPS / STEP_MODE + 1];
//^ only single bytes, so don't go for crazy large pulses as will overflow!

//timings for pulses (us == microseconds)
uint32_t last_pulse_us; //initialised in go_home();
uint16_t pulse_interval_us;

//serial
uint8_t serial_buffer[BUF_LENGTH];
uint8_t msg_bytes = 0;
uint8_t buf_pointer = 0;

//experimental/debug variables
#define DEBUG_MODE 0
uint16_t counter = 0;

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
    Serial.begin(115200);

    for (uint16_t i = 0; i < NUM_ACCEL_OR_DECEL_STEPS / STEP_MODE + 1; i++){
        accel_pulse_us[i] = smootherstep_interp((float) i / (NUM_ACCEL_OR_DECEL_STEPS / STEP_MODE),
                                          MIN_STEP_SPEED, MAX_STEP_SPEED) / STEP_MODE;
    }

    go_home();
}

void loop() {
    //no blocking functions allowed, this needs to zip
    //around as fast as possible for the stepper steps
    if (Serial.available()){
        uint8_t b = Serial.read();
        if (msg_bytes){ //we are mid-message...
            serial_buffer[buf_pointer++] = b;
            if (buf_pointer == msg_bytes){
                handle_message();
                msg_bytes = 0;
            }
        } else { //first byte of a message...
            msg_bytes = b;
            buf_pointer = 0;
        }
    }
    //if not at stepper target and due to pulse, let's pulse
    if (x_pos != x_target && (micros() - last_pulse_us) >= pulse_interval_us){
        if (DEBUG_MODE && !(counter++ & 0xff)){
            Serial.print(0); Serial.print(" "); Serial.print(300); Serial.print(" ");
            Serial.println(pulse_interval_us);
        }
        //make it look like we pulsed at the right time
        last_pulse_us += pulse_interval_us;
        //pulse the motor
        step_stepper(x_pos < x_target ? RIGHT : LEFT);

        //adjust pulse_interval_us according to if decelerating or accelerating
        uint16_t steps_from_target = x_pos < x_target ? x_target - x_pos : x_pos - x_target;
        if (x_target == 0){
            pulse_interval_us = MIN_STEP_SPEED / STEP_MODE;
        } else if (steps_from_target < NUM_ACCEL_OR_DECEL_STEPS){
            //decelerating
            if (steps_from_target < pulses_since_accel_start)
            pulse_interval_us = accel_pulse_us[steps_from_target / STEP_MODE];
            else
            pulse_interval_us = MIN_STEP_SPEED / STEP_MODE;
        } else if (pulses_since_accel_start < NUM_ACCEL_OR_DECEL_STEPS){
            //accelerating
            pulse_interval_us = accel_pulse_us[pulses_since_accel_start++ / STEP_MODE];
        }
        //if going home, we listen for endstop instead of blindly moving :)
        if (x_target == 0){
            if (digitalRead(ENDSTOP)){
                x_pos = 0;
                Serial.write(CODE_X_TARGET_REACHED);
                //this is just so that the arm doesn't wait on the endstop as it is drying
                x_target = 500;
           }
        } else {
            x_pos += x_pos < x_target ? 1 : -1;
            if (x_pos == x_target)
            Serial.write(CODE_X_TARGET_REACHED);
       }
    }
}

//these interpolation functions are for smoothing the acceleration
//only one is actually used to populate the accel_pulse_us array
uint16_t linear_interp(float x, int16_t a, int16_t b){
    //basic linear, x in range [0¬1]
    return a + x * (b - a);
    // ^ ~20us so fast enough to calculate on fly
}

uint16_t smoothstep_interp(float x, int16_t a, int16_t b){
    //3x^2 - 2x*3
    return a + (3*x*x - 2*x*x*x) * (b - a);
    // ^ ~80us to complete! too slow to calculate on fly
}

uint16_t smootherstep_interp(float x, int16_t a, int16_t b){
    //6x^5 - 15x^4 + 10x^3
    return  a + (6*x*x*x*x*x - 15*x*x*x*x + 10*x*x*x) * (b - a);
    // ^~142us to complete! too slow to calculate on fly
}

void handle_message(){
    switch (serial_buffer[0]){
        case 0: //home
            if (msg_bytes == 1){
                Serial.write(CODE_NORMAL);
                go_home();
            } else {
                Serial.write(CODE_INVALID_NEBS);
            } break;
        case 1: //set motors
            if (msg_bytes == 2){
                if (serial_buffer[1]) enable_motors();
                else disable_motors();
                Serial.write(CODE_NORMAL);
            } else {
                Serial.write(CODE_INVALID_NEBS);
            } break;
        case 2: //set grabber
            if (msg_bytes == 2){
                Serial.write(CODE_NORMAL);
                set_grabber(serial_buffer[1]);
            } else {
                Serial.write(CODE_INVALID_NEBS);
            } break;
        case 3: //move to
            if (msg_bytes == 7){
                int16_t x = INT_FROM_BYTES(serial_buffer[1], serial_buffer[2]),
                        y = INT_FROM_BYTES(serial_buffer[3], serial_buffer[4]),
                        z = INT_FROM_BYTES(serial_buffer[5], serial_buffer[6]);
                if (X_IN_RANGE(x) && Y_IN_RANGE(y) && Z_IN_RANGE(z)){
                    Serial.write(CODE_NORMAL);
                    move_to(x,y,z);
                } else {
                    Serial.write(CODE_INVALID_INPUT);
                }
            } else {
                Serial.write(CODE_INVALID_NEBS);
            } break;
        default:
            Serial.write(CODE_INVALID_TYPE);
            break;
    }
}

void move_to(int16_t x, int16_t y, int16_t z){
    //move to position (x,y,z), limits are described by the *_IN_RANGE(*) macros
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
    //if already there, send appropriate reply
    if (x_pos == x_target) Serial.write(CODE_X_TARGET_REACHED);
    //need to pretend we just pulsed so timeings re-baised from here
    last_pulse_us = micros();
    pulses_since_accel_start = 0;
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
    d -= GRIPPER_LENGTH; //don't need to go as far as gripper has length
    z += GRIPPER_HEIGHT; //need to go higher as gripper has height
    z -= PIVOT_HEIGHT;   //don't need to go as high as servo pivots already in air
    d -= GRIPPER_LENGTH; //don't need to go as far as gripper has length
    float Z = atan2(z, d);
    float A = acos((sq((int32_t)SEGMENT_LENGTH_1) + sq((int32_t)d) + sq((int32_t)z) - sq((int32_t)SEGMENT_LENGTH_2)) /
                   (2 * SEGMENT_LENGTH_1 * sqrt(sq((int32_t)d) + sq((int32_t)z))));
    float B = acos((sq((int32_t)SEGMENT_LENGTH_2) + sq((int32_t)d) + sq((int32_t)z) - sq((int32_t)SEGMENT_LENGTH_1)) /
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
    pulses_since_accel_start = 0;
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
