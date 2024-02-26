#include <SparkFun_TB6612.h>

#define PWMA 10
#define AIN1 9 //Assign the motor pins
#define AIN2 8
#define BIN1 7
#define BIN2 6
#define PWMB 5
#define STBY 11

// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.

Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY);


int P, D, I, previousError, PIDvalue, error;
int lsp, rsp;
int lfspeed = 255;

float Kp = 0; // Proportional gain
float Ki = 0; // Integral gain
float Kd = 0; // Derivative gain

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


void loop()
{
  while (digitalRead(3)) {}
  delay(1000);
  calibrate();
  while (digitalRead(4)) {}
  delay(2000);
  followLine();
}

void followLine(){
  while (1)
  {
    if (analogRead(A0) > threshold[0] && analogRead(A7) < threshold[7])
    {
      motor1.drive(-70);
      motor2.drive(100);
      while (analogRead(A7) < threshold[7] && analogRead(A6) < threshold[6] && analogRead(A5) < threshold[5] && analogRead(A4) < threshold[4] && analogRead(A3) < threshold[3] && analogRead(A2) < threshold[2] && analogRead(A1) < threshold[1] && analogRead(A0) < threshold[0]){
          rightTurn();
        }
    }

    else if (analogRead(A7) > threshold[7] && analogRead(A0) < threshold[0])
    {
      motor1.drive(100);
      motor2.drive(-70);
      while (analogRead(A7) < threshold[7] && analogRead(A6) < threshold[6] && analogRead(A5) < threshold[5] && analogRead(A4) < threshold[4] && analogRead(A3) < threshold[3] && analogRead(A2) < threshold[2] && analogRead(A1) < threshold[1] && analogRead(A0) < threshold[0]){
          leftTurn();
        } 
    }

    else if (analogRead(A1) > threshold[1] && analogRead(A6) < threshold[6] )
    {
      motor1.drive(-50);
      motor2.drive(100);
      while (analogRead(A7) < threshold[7] && analogRead(A6) < threshold[6] && analogRead(A5) < threshold[5] && analogRead(A4) < threshold[4] && analogRead(A3) < threshold[3] && analogRead(A2) < threshold[2] && analogRead(A1) < threshold[1] && analogRead(A0) < threshold[0]){
          rightTurn();
        }
    }

    else if (analogRead(A6) > threshold[6] && analogRead(A1) < threshold[1])
    {
      motor1.drive(100);
      motor2.drive(-50);
      while (analogRead(A7) < threshold[7] && analogRead(A6) < threshold[6] && analogRead(A5) < threshold[5] && analogRead(A4) < threshold[4] && analogRead(A3) < threshold[3] && analogRead(A2) < threshold[2] && analogRead(A1) < threshold[1] && analogRead(A0) < threshold[0]){
          leftTurn();
        }
    }

    else if (analogRead(A3) > threshold[3] && analogRead(A4) > threshold[4])
    {
      motor1.drive(180);
      motor2.drive(180);
    }

    else if (analogRead(A3) < threshold[3] || analogRead(A4) < threshold[4])
    {
      int avgReading = ((analogRead(A3) + analogRead(A4)) / 2);
    
     // Calculate Kp and Kd based on the average reading
      Kp = 0.01 * (1000 - avgReading);
      Kd = 10 * Kp;
      Ki = 0.0001;
      linefollow();
    }
  }
}

void rightTurn(){
  motor1.drive(-100);
  motor2.drive(100);
}

void leftTurn(){
  motor1.drive(100);
  motor2.drive(-100);
}

void linefollow()
{
  int error = (analogRead(A5) - analogRead(A2));

  P = error;
  I = I + error;
  D = error - previousError;

  PIDvalue = (Kp * P) + (Ki * I) + (Kd * D);
  previousError = error;

  lsp = lfspeed - PIDvalue;
  rsp = lfspeed + PIDvalue;

  if (lsp > 100) {
      lsp = 100;
  }
  if (lsp < -10) {
      lsp = -10;
  }
  if (rsp > 100) {
      rsp = 100;
  }
  if (rsp < -10) {
      rsp = -10;
  }
  motor1.drive(rsp);
  motor2.drive(lsp);

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