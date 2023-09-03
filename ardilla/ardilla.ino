#include <servo.h>

Servo servo;

/****************************************
 * General constant
 */


/****************************************
 * Engines
 */

//Motor driver L289N
const  int enAPin = 6; // Left  motor PWM speed control
const  int in1Pin = 7; // Left  motor  Direction  1
const int motorIzqAd
const  int in2Pin = 5; // Left  motor  Direction  2
const  int in3Pin = 4; // Right  motor  Direction  1
const  int in4Pin = 2; // Right  motor  Direction  2
const  int enBPin = 3; // Rigfht  motor PWM speed control
enum Motor{LEFT, RIGHT};

//  Set motor speed: 255 full ahead,-255 full reverse ,0  stop
void go(enum Motor m,int speed){
  digitalWrite (m == LEFT ? in1Pin :  in3Pin,  speed>0 ?  HIGH : LOW );
  digitalWrite (m == LEFT ? in2Pin :  in4Pin,  speed<= 0 ? HIGH : LOW );
  analogWrite(m == LEFT ? enAPin : enBPin, speed<0 ?-speed :  speed  );
}

//    Initial   motor  test :
//left   motor forward  then  back
//right  motor forward  then  back
void testMotors()
{
  static int speed[8]= {128, 210, 128, 0,-128, -210, -128, 0};
  go(RIGHT, 0);
  for(unsigned char i  = 0;  i<8 ;  i ++)
    go(LEFT, speed[i ]),   delay (200);
  for(unsigned char i  = 0  ;  i<8 ;  i ++)
    go(RIGHT, speed[i ]),   delay (200);
}

/****************************************
 * Ultrasonic sensor
 */

//  Read distance  from  the   ultrasonic   sensor ,   return   distance   in  mm
//
//  Speed of sound in dry  air,  20C is  343 m/s
//  pulseIn  returns  time in  microseconds (10ˆ−6)
//  2d = p*10ˆ−6 s*343 m/s  = p*0.00343 m = p*0.343  mm/us
unsigned int readDistance()
{
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long period = pulseIn (echoPin, HIGH);
  return period * 343 / 2000;
}

#define NUM_ANGLES 7
unsigned char sensorAngle[NUM_ANGLES] = {60, 70, 80, 90, 100, 110, 120 };
unsigned int distance[NUM_ANGLES];


//  Scan the  area  ahead by  sweeping  the   ultrasonic   sensor   left   and  right
//  and  recording  the   distance  observed .This  takes  a  reading ,  then
//   sends  the  servo  to  the  next  angle .Call   repeatedly  once  every  50 ms or  so .
void readNextDistance()
{
  static unsigned char angleIndex = 0;
  static signed char step = 1;

  distance[angleIndex] = 400; //readDistance();
  angleIndex += step;

  if (angleIndex == NUM_ANGLES -1) step = -1;
  else if (angleIndex == 0) step = 1;

  servo.write(sensorAngle[angleIndex]);
}

/****************************************
 * General
 */
void setup() {
  // put your setup code here, to run once:
  pinMode(enAPin, OUTPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  pinMode(enBPin, OUTPUT);
  testMotors();

  servo.attach(servoPin);
  servo.write(90);

  go(LEFT, 0);
  go(RIGHT, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  readNextDistance();

  // See if something is too close at any angle
  unsigned char tooClose = 0;
  for(unsigned char i = 0; i < NUM_ANGLES ;i++)
    if (distance[i] < 300)
      tooClose = 1;
  
  if (tooClose) {
    // Something is nearby: back up left
    go(LEFT, -120); // 180
    go(RIGHT, -60); // 80
  } else {
    // Nothing wrong: go foward
    go(LEFT, 180); // 255
    go(RIGHT, 180); // 255
  }

  // Check next direction in 50 ms just for test 1000ms
  delay(100);
}
