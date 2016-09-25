/*****************************************************************************/

// HackCooper: 24h Hackathon
// Project: Spark Core tricycle robot
// Input: Flex Sensor + Accelerometer

/*****************************************************************************/
//	Function:	 Flex and twist robot HackCooper
//  Hardware:    particle core, flex sensor, Grove - 3-Axis Digital Accelerometer(±1.5g)
//	IDE: Particle Dev
//	Author:	 Team Flex and Twist robot, Quirin Koerner
//	Date: 	 Sept 25,2016
//	Version: v1.0
//
//  The MMA7660 library is copied from by www.seeedstudio.com
//
/*******************************************************************************/
//
//  As this code is from a 24h Hackathon exspect errors and bad style :)
//
/*****************************************************************************/

#include "Particle.h" //particle stuff
#include "application.h" //particle stuff
#include "MMA7660.h" //Acc Sensor library

SYSTEM_MODE(SEMI_AUTOMATIC);

// bools to activate debugging levels
bool debug = FALSE;
bool debug2 = FALSE;
bool debug3 = TRUE;

int voltage_limiter = 255; // Set the max analog output level. Needed with motor below 6V. Allowed values [0-255]

int motor1 = A7; //left motor
int motor2 = A6; //right motor
int BendingSensor = A0; //reads deflection/flex sensor, The flex sensor is basically a resistor changeing it's value if deflected

// To get better results for the flex sensor, it gets calibrated once the particle core starts (average over first 10 values)
int initBending = 0; //counter for init Sensor
int trashholdBending = 0; // stores the sum of values
int calibBending = 0; // actual calibration value

MMA7660 accelemeter; //creates MMA7660 (Accelerometer) object, see library

void setup() {
  //This function is only called once at startup

  attachInterrupt(D4, connect, FALLING); // Interrupt for calling the wifi connect request (3.3V -> 0V)

  pinMode(motor1, OUTPUT); //motor left setup
  pinMode(motor2, OUTPUT); //motor right setup

  //Start Serial interface fpr debugging
  Serial.begin(9600);
  Serial.print("hello world in init");

  accelemeter.init(); //Start Acc Sensor

}

void loop() {
  //main function, endless loop

  delay(100);  //There will be new values every 100ms

  //////////////////////////////////////////////////////
  // read Acc values and debug
  //////////////////////////////////////////////////////

  float x,y,z;
  accelemeter.getAcceleration(&x,&y,&z);
  //debug message for Acc values
  if(debug2)
  {
  Serial.print("Values: ");
  Serial.print("X:");
  Serial.print(x);
  Serial.print(" Y:");
  Serial.print(y);
  Serial.print(" Z:");
  Serial.println(z);
  }

  //////////////////////////////////////////////////////
  //read deflection sensor values and debug
  int bend_vel = 0;
  bend_vel = analogRead(BendingSensor) - calibBending;

  /// calibrate the sensor to zero
  if(calibBending == 0) //do online if there is no calibration value
  {
    if(initBending == 10) //averrage over 10 values (the first ones)
    {
      calibBending = trashholdBending / 10;
    }
    else
    {
      trashholdBending = trashholdBending + bend_vel; //sum up
      initBending = initBending + 1;
      if(debug)
      {
        Serial.print(initBending);
        Serial.println(" / 10");
      }
    }
  }
  if(debug)
  {
    Serial.print("Bending Value: ");
    Serial.println(bend_vel);
  }

  /////////////////////////////////////////////////////
  // Move car
  /////////////////////////////////////////////////////
  // the car is moved forward by the felx sensor. The Acc values is added/subtracted to each of the motors (crossover) to turn the car.
  ////////////////////////////////////////////////////

  float x_vel = bend_vel / 1000.00 * (-1); //normalize bend_vel (not a good solution)

  //The Acc gives back values from 0 to 3, what equals -1.5g to 1.5g so we have to shift it
  if (x >= 1.5)
    {
      x = x - 3.0;
    }

  float a_vel = x * 1.00;

  if(debug3)
  {
    Serial.print("VEL values: X: ");
    Serial.print(x_vel);
    Serial.print(" TURN angle: A: ");
    Serial.println(a_vel);
  }

  //Combine the turn values and the forward values and weight them
  float left_vel = 200 * x_vel - a_vel * 200 + 50;
  float right_vel = 200.0 * x_vel + a_vel * 200 + 50;

  // check that the maximal voltages is not exceeded (important with less then 6V motors)
  if (left_vel > voltage_limiter)
  {left_vel = voltage_limiter;}
  if (right_vel > voltage_limiter)
  {right_vel = voltage_limiter;}

  //write analog value to pins
  analogWrite(motor1, left_vel);
  analogWrite(motor2, right_vel);

  if(debug3)
  {
    Serial.print("left: ");
    Serial.print(left_vel);
    Serial.print("right: ");
    Serial.println(right_vel);
  }
}

void connect() {
  //This interrupt function establishes the Wireless LAN connection
  if (Spark.connected() == false) {
    Spark.connect();}
  }
