#include <SparkFun_TB6612.h>

#define PWMA 10
#define AIN1 9 //Assign the motor pins
#define AIN2 8
#define BIN1 7
#define BIN2 6
#define PWMB 5
#define STBY 11

const int offsetA = 1;
const int offsetB = 1;

// define motor object with chosen offsets
Motor motor1(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motor2(BIN1, BIN2, PWMB, offsetB, STBY);

// pre-calculate constants for speed and turning
const int lfspeed = 255;
const int rightTurnSpeed = -100;
const int leftTurnSpeed = 100;

// declare and initialize threshold array outside loop
int minValues[8], maxValues[8], threshold[8];

void setup() {
  Serial.begin(9600);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);

  
}

void loop() {
  while (digitalRead(3)) {}
  delay(1000);
  calibrate();

  while (digitalRead(4)) {}
  delay(1000);
  followLine();
}

void followLine() {
  while (1) {
    int sensorReadings[8]; // read all sensors at once

    // read all sensor values into an array
    for (int i = 0; i < 8; i++) {
      sensorReadings[i] = analogRead(i);
    }

    // check line position based on readings
    if (sensorReadings[0] > threshold[0] && sensorReadings[7] < threshold[7]) {
      // right turn
      motor1.drive(rightTurnSpeed);
      motor2.drive(lfspeed);
      while (checkLine(sensorReadings)) {}
    } else if (sensorReadings[7] > threshold[7] && sensorReadings[0] < threshold[0]) {
      // left turn
      motor1.drive(lfspeed);
      motor2.drive(leftTurnSpeed);
      while (checkLine(sensorReadings)) {}
    } else if (sensorReadings[1] > threshold[1] && sensorReadings[6] < threshold[6]) {
      // slight right turn
      motor1.drive(-50); // adjust speed as needed
      motor2.drive(lfspeed);
      while (checkLine(sensorReadings)) {}
    } else if (sensorReadings[6] > threshold[6] && sensorReadings[1] < threshold[1]) {
      // slight left turn
      motor1.drive(lfspeed);
      motor2.drive(-50); // adjust speed as needed
      while (checkLine(sensorReadings)) {}
    } else if (sensorReadings[3] > threshold[3] && sensorReadings[4] > threshold[4]) {
      // both sensors on line, move forward
      motor1.drive(lfspeed);
      motor2.drive(lfspeed);
    } else {
      // implement your desired behavior when off the line (e.g., stop, reverse)
      motor1.drive(0);
      motor2.drive(0);
    }
  }
}

bool checkLine(int readings[]) {
  // check if any sensor reading is below the threshold
  for (int i = 0; i < 8; i++) {
    if (readings[i] < threshold[i]) {
      return true; // still on the line
    }
  }
  return false; // off the line
}

void calibrate()
{
  for ( int i = 0; i < 8; i++)
  {
    minValues[i] = analogRead(i);
    maxValues[i] = analogRead(i);
  }
  
  for (int i = 0; i < 600; i++)
  {
    motor1.drive(-70);
    motor2.drive(50);

    for ( int i = 0; i < 8; i++)
    {
      if (analogRead(i) < minValues[i])
      {
        minValues[i] = analogRead(i);
      }
      if (analogRead(i) > maxValues[i])
      {
        maxValues[i] = analogRead(i);
      }
    }
  }

  for ( int i = 0; i < 8; i++)
  {
    threshold[i] = (minValues[i] + maxValues[i]) / 2;
    Serial.print(threshold[i]);
    Serial.print("   ");
  }
  Serial.println();
  
  motor1.drive(0);
  motor2.drive(0);
}