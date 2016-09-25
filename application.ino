// Hackcooper Spark Core tricycle robot
#include "Particle.h" //particle stuff
#include "application.h" //
#include "MMA7660.h" //Acc Sensor library

SYSTEM_MODE(SEMI_AUTOMATIC);

bool debug = FALSE;
bool debug2 = FALSE;
bool debug3 = TRUE;
int voltage_limiter = 255;

int motor1 = A7; //left motor1
int motor2 = A6; //right motor
int BendingSensor = A0; //reads deflection resistor
int initBending = 0; //counter for init Sensor
int trashholdBending = 0;
int calibBending = 0;

MMA7660 accelemeter;

void setup() {
  attachInterrupt(D4, connect, FALLING);
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  Serial.begin(9600);
  Serial.print("hello world in init");
  accelemeter.init();

}

void loop() {
  /*
  analogWrite(motor1, 38);   // Turn ON the LED pins
  digitalWrite(led2, HIGH);
  delay(4000);               // Wait for 1000mS = 1 second
  analogWrite(motor1, 0);    // Turn OFF the LED pins
  digitalWrite(led2, LOW);
  delay(1000);               // Wait for 1 second in off mode
  */

  // read Acc values and debug
  float x,y,z;
  delay(100);  //There will be new values every 100ms
  accelemeter.getAcceleration(&x,&y,&z);
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

  //read deflection sensor values and debug
  int bend_vel = 0;
  bend_vel = analogRead(BendingSensor) - calibBending;

  /////////////////////////////////////////////////////
  /// calib the sensor to zero
  if(calibBending == 0)
  {
    if(initBending == 10)
    {
      calibBending = trashholdBending / 10;
    }
    else
    {
      trashholdBending = trashholdBending + bend_vel;
      initBending = initBending + 1;
      Serial.print(initBending);
      Serial.println(" / 10");
    }
  }
  if(debug)
  {
  Serial.print("Bending Value: ");
  Serial.println(bend_vel);
  }
  /////////////////////////////////////////////////////
  // Move car

  float x_vel = bend_vel / 1000.00 * (-1);
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
  float left_vel = 200 * x_vel - a_vel * 200 + 50;
  float right_vel = 200.0 * x_vel + a_vel * 200 + 50;

  if (left_vel > voltage_limiter)
  {left_vel = voltage_limiter;}
  if (right_vel > voltage_limiter)
  {right_vel = voltage_limiter;}

  analogWrite(motor1, left_vel);
  analogWrite(motor2, right_vel);

  Serial.print("left: ");
  Serial.print(left_vel);
  Serial.print("right: ");
  Serial.println(right_vel);

}

void connect() {
  if (Spark.connected() == false) {
    Spark.connect();}
  }
