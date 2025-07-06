#include "Wire.h"
#include "Adafruit_TCS34725.h"

#define IR_SENSOR_RIGHT 11 
#define IR_SENSOR_LEFT 12
#define MOTOR_SPEED 200
#define COLOR_SENSOR_SDA  A4  // Adjust based on your SDA pin connection
#define COLOR_SENSOR_SCL  A5  // Adjust based on your SCL pin connection
#define STOP_COLOR  0x00FF00  // Adjust for your desired RGB color value (Green in this example)
#define STOP_DURATION  5000  // Stop duration in milliseconds (5 seconds in this example)

//Right motor
int enableRightMotor=6;
int rightMotorPin1=7;
int rightMotorPin2=8;

//Left motor
int enableLeftMotor=5;
int leftMotorPin1=9;
int leftMotorPin2=10;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_600MS, TCS34725_GAIN_1X);
unsigned long stopStartTime = 0;  // Variable to store stop start time

void setup()
{
  //The problem with TT gear motors is that, at very low pwm value it does not even rotate.
  //If we increase the PWM value then it rotates faster and our robot is not controlled in that speed and goes out of line.
  //For that we need to increase the frequency of analogWrite.
  //Below line is important to change the frequency of PWM signal on pin D5 and D6
  //Because of this, motor runs in controlled manner (lower speed) at high PWM value.
  //This sets frequecny as 7812.5 hz.
  TCCR0B = TCCR0B & B11111000 | B00000010 ;
  
  // put your setup code here, to run once:
  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  
  pinMode(enableLeftMotor, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  pinMode(IR_SENSOR_RIGHT, INPUT);
  pinMode(IR_SENSOR_LEFT, INPUT);
  rotateMotor(0,0);   

  if (tcs.begin()) {
    Serial.println("TCS34725 Found!");
  } else {
    Serial.println("No TCS34725 found, check connections!");
    // Add error handling here
  }
}


void loop()
{

  int rightIRSensorValue = digitalRead(IR_SENSOR_RIGHT);
  int leftIRSensorValue = digitalRead(IR_SENSOR_LEFT);

  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  // Check for stop color
  if (g >= STOP_COLOR >> 8 && g <= STOP_COLOR & 0x00FF) {
    Serial.println("Stop color detected!");
    stopStartTime = millis();  // Record stop start time

    // Stop motors for the defined duration
    while (millis() - stopStartTime < STOP_DURATION) {
      rotateMotor(0, 0);  // Stop both motors
      delay(10);  // Short delay to avoid busy waiting
    }
  }
  else{

   //If none of the sensors detects black line, then go straight
   if (rightIRSensorValue == LOW && leftIRSensorValue == LOW)
   {
     rotateMotor(MOTOR_SPEED, MOTOR_SPEED);
   }
   //If right sensor detects black line, then turn right
   else if (rightIRSensorValue == HIGH && leftIRSensorValue == LOW )
   {
       rotateMotor(-MOTOR_SPEED, MOTOR_SPEED); 
   }
   //If left sensor detects black line, then turn left  
   else if (rightIRSensorValue == LOW && leftIRSensorValue == HIGH )
   {
       rotateMotor(MOTOR_SPEED, -MOTOR_SPEED); 
   } 
   //If both the sensors detect black line, then stop 
   else 
   {
     rotateMotor(0, 0);
   }
}
}


void rotateMotor(int rightMotorSpeed, int leftMotorSpeed)
{
  
  if (rightMotorSpeed < 0)
  {
    digitalWrite(rightMotorPin1,LOW);
    digitalWrite(rightMotorPin2,HIGH);    
  }
  else if (rightMotorSpeed > 0)
  {
    digitalWrite(rightMotorPin1,HIGH);
    digitalWrite(rightMotorPin2,LOW);      
  }
  else
  {
    digitalWrite(rightMotorPin1,LOW);
    digitalWrite(rightMotorPin2,LOW);      
  }

  if (leftMotorSpeed < 0)
  {
    digitalWrite(leftMotorPin1,LOW);
    digitalWrite(leftMotorPin2,HIGH);    
  }
  else if (leftMotorSpeed > 0)
  {
    digitalWrite(leftMotorPin1,HIGH);
    digitalWrite(leftMotorPin2,LOW);      
  }
  else 
  {
    digitalWrite(leftMotorPin1,LOW);
    digitalWrite(leftMotorPin2,LOW);      
  }
  analogWrite(enableRightMotor, abs(rightMotorSpeed));
  analogWrite(enableLeftMotor, abs(leftMotorSpeed));    
}
