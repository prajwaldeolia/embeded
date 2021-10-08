#include <PS2X_lib.h>  //for v1.6
#include <Servo.h>
Servo s1;
#define dir1 5
#define pwm1 4
#define dir2 7
#define pwm2 6
#define dir3 9
#define pwm3 8
#define dir4 24
#define pwm4 12

//#define pwm9 6
//#define dir9 5

#define trig1 41
#define echo1 39
#define trig2 35
#define echo2 37
#define trig3 33
#define echo3 31
#define trig4 27
#define echo4 29

PS2X ps2x; // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning
//you must always either restart your Arduino after you conect the controller,
//or call config_gamepad(pins) again after connecting the controller.
int error = 0;
byte type = 0;
byte vibrate = 0;
int sval1 = 0;
int sval2 = 0;

float input, output;
float p, i, d;
float err, i_err, last_err;
int threshhold;
double kp = 0.8;                                  //constant values
double kd = 0.4;
double ki = 0.004;
double dt;

int analog1 = 0;
int analog2 = 0;

#define ps2_data 42
#define ps2_cmd  44
#define ps2_clk  48
#define ps2_atn  46

int mapped;
int pwm = 50;
int pulses = 0;
#define ENC_COUNT_REV 80
#define ENC_INA 25
#define ENC_INB 23

int state;
int pickie = 0;
int count = 0;
int laststate;
int counter = 0;
int clockwise = 0;
int flag = 0;
int pos = 0;
int tflag = 0;

void A_CHANGE();
float moveFW(float);
float moveBW(float);
float moveRight(float);
float moveLeft(float);
float moveLeft_d();
float moveRight_d();
float moveRightBW_d();
float moveLeftBW_d();
float stay();
int PID(float, double, int);
float rotateCW();
float rotateACW();
int ultdist(int, int);
void throwing();
void back();
void setup() {
  Serial.begin(57600);


  error = ps2x.config_gamepad(28, 24, 26, 22, false, false); //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error

  if (error == 0) {
    Serial.println("Found Controller, configured successful");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Go to www.billporter.info for updates and to report bugs.");
  }

  else if (error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");

  else if (error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if (error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");



  type = ps2x.readType();
  switch (type) {
    case 0:
      Serial.println("Unknown Controller type");
      break;
    case 1:
      Serial.println("DualShock Controller Found");
      break;
    case 2:
      Serial.println("GuitarHero Controller Found");
      break;
  }
  pinMode(pwm1, OUTPUT);
  pinMode(pwm2, OUTPUT);
  pinMode(pwm3, OUTPUT);
  pinMode(pwm4, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(dir3, OUTPUT);
  pinMode(dir4, OUTPUT);

  s1.attach(7);

  pinMode(ENC_INA, INPUT);
  pinMode(ENC_INB, INPUT);
  //  pinMode(pwm9, OUTPUT);
  //  pinMode(dir9, OUTPUT);
  delay(30);
  analogWrite(pwm1, 0);
  analogWrite(pwm2, 0);
  analogWrite(pwm3, 0);
  analogWrite(pwm4, 0);
  //  analogWrite(pwm9, 0);
  attachInterrupt(digitalPinToInterrupt(ENC_INA), A_CHANGE, RISING);
  delay(30);




}

void loop() {
  /* You must Read Gamepad to get new values
    Read GamePad and set vibration values
    ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
    if you don't enable the rumble, use ps2x.read_gamepad(); with no values

    you should call this at least once a second
  */


  Serial.println(pulses);
  if (error == 1) //skip loop if no controller found
    return;
  else { //DualShock Controller

    ps2x.read_gamepad(false, false);         //read controller and set large motor to spin at 'vibrate' speed

    /*if (flag == 0 && (ps2x.ButtonPressed(PSB_RED)))
      {
      grip1(130);
      }
      if (flag == 0 && (ps2x.ButtonPressed(PSB_GREEN)))
      {
      grip2(82);
      }*/
    if (flag == 0 && tflag == 0 && (ps2x.Button(PSB_R1)) && (ps2x.Button(PSB_BLUE)))
    {
      tflag = 1;
    }
    if (tflag == 1)
    {
      //press continuously to throw
      Serial.println("throwing");

      // Update RPM value every second
      throwing();
    }
    if (tflag == 2)
    {
      back();
    }
    /* if ((((flag == 0) && ((pulses > 2) && (pulses < 5) ) ) && (pickie == 0 )) || (ps2x.Button(PSB_L1)) )
      {
       if ( (ps2x.Button(PSB_SQUARE)) || ((pulses > 2) && (pulses < 5) ) )
       {
         //press continuously to throw
         Serial.println("picking");

         // Update RPM value every second
         pick();
       }

      }

      if (tflag == 0)
      {
      analogWrite(pwm9, 0);
      flag = 0;
      pickie = 0;
      }*/

    if (ps2x.Button(PSB_L1))
    {
      analog1 = ps2x.Analog(PSS_LY);
      // Serial.print(analog1);

      analog2 = ps2x.Analog(PSS_RX);
      //Serial.print(analog2);

      if ((analog1 < 130 && analog1 > 126) && (analog2 < 130 && analog2 > 126)) //forward causing motors zero!!!!
      {
        stay();
      }
      if ((analog1 > 130 || analog1 < 126) && (analog2 > 130 || analog2 < 126)) //forward causing motors zero!!!!
      {
        stay();
      }
      else {

        if (analog1 <= 126)   //forward function
        {
          sval1 = map(analog1, 0, 126, pwm, 0);
          moveFW(sval1);
        }
        if (analog1 >= 130)     //backward function
        {

          sval1 = map(analog1, 130, 255, 0, pwm);
          moveBW(sval1);

        }

        if (analog2 <= 126)   //left function
        {

          sval2 = map(analog2, 0, 126, pwm, 0);
          moveLeft(sval2);

        }
        if (analog2 >= 130)     //right function
        {

          sval2 = map(analog2, 130, 255, 0, pwm);
          moveRight(sval2);

        }
      }
    }
    else if (ps2x.Button(PSB_SQUARE) && ps2x.Button(PSB_R2))
    {
      rotateCW();
    }
    else if (ps2x.Button(PSB_SQUARE) && ps2x.Button(PSB_L2))
    {
      rotateACW();
    }
    else if (ps2x.Button(PSB_PAD_RIGHT) && ps2x.Button(PSB_L1))
    {
      moveRight_d();
    }
    else if (ps2x.Button(PSB_PAD_LEFT) && ps2x.Button(PSB_L1))
    {
      moveLeft_d();
    }
    else if (ps2x.Button(PSB_PAD_RIGHT) && ps2x.Button(PSB_R1))
    {
      moveRightBW_d();
    }
    else if (ps2x.Button(PSB_PAD_LEFT) && ps2x.Button(PSB_R1))
    {
      moveLeftBW_d();
    }

    else
    {
      analogWrite(pwm1, 0);
      analogWrite(pwm2, 0);
      analogWrite(pwm3, 0);
      analogWrite(pwm4, 0);
    }


  }

  delay(50);
}
float moveFW(float a)               // forward
{
  Serial.println("forward");
  digitalWrite(dir1, HIGH);
  digitalWrite(dir2, HIGH);
  digitalWrite(dir3, HIGH);
  digitalWrite(dir4, HIGH);

  analogWrite(pwm1, a);
  analogWrite(pwm2, a);
  analogWrite(pwm3, a);
  analogWrite(pwm4, a);

}

float moveBW(float a)               // backward
{
  Serial.println("moving BW");
  digitalWrite(dir1, LOW);
  digitalWrite(dir2, LOW);
  digitalWrite(dir3, LOW);
  digitalWrite(dir4, LOW);

  analogWrite(pwm1, a);
  analogWrite(pwm2, a);
  analogWrite(pwm3, a);
  analogWrite(pwm4, a);

}

float stay()             // stopping
{
  Serial.println("stopped");
  analogWrite(pwm1, 0);
  analogWrite(pwm2, 0);
  analogWrite(pwm3, 0);
  analogWrite(pwm4, 0);

}

float moveRight(float a)
{
  Serial.println("moving right");
  digitalWrite(dir1, LOW);
  digitalWrite(dir2, HIGH);
  digitalWrite(dir3, LOW);
  digitalWrite(dir4, HIGH);

  analogWrite(pwm1, a);
  analogWrite(pwm2, a);
  analogWrite(pwm3, a);
  analogWrite(pwm4, a);

}
float moveLeft(float a)

{
  Serial.println("moving left");
  digitalWrite(dir1, HIGH);
  digitalWrite(dir2, LOW);
  digitalWrite(dir3, HIGH);
  digitalWrite(dir4, LOW);

  analogWrite(pwm1, a);
  analogWrite(pwm2, a);
  analogWrite(pwm3, a);
  analogWrite(pwm4, a);

}

float rotateCW()
{
  Serial.println("rotation CW");
  digitalWrite(dir1, HIGH);
  digitalWrite(dir2, HIGH);
  digitalWrite(dir3, LOW);
  digitalWrite(dir4, LOW);

  analogWrite(pwm1, 40);
  analogWrite(pwm2, 40);
  analogWrite(pwm3, 40);
  analogWrite(pwm4, 40);


}

float rotateACW()
{
  Serial.println("rotation ACW");
  digitalWrite(dir1, LOW);
  digitalWrite(dir2, LOW);
  digitalWrite(dir3, HIGH);
  digitalWrite(dir4, HIGH);

  analogWrite(pwm1, 40);
  analogWrite(pwm2, 40);
  analogWrite(pwm3, 40);
  analogWrite(pwm4, 40);

}

float moveRight_d()           //diagonal right movement

{
  Serial.println("sashi buri");
  digitalWrite(dir1, HIGH);
  digitalWrite(dir2, HIGH);
  digitalWrite(dir3, HIGH);
  digitalWrite(dir4, HIGH);

  analogWrite(pwm1, 0);
  analogWrite(pwm2, 40);
  analogWrite(pwm3, 0);
  analogWrite(pwm4, 40);
}
float moveLeft_d()             //diagonal left movement
{
  Serial.println("sashi buri1");
  digitalWrite(dir1, HIGH);
  digitalWrite(dir2, HIGH);
  digitalWrite(dir3, HIGH);
  digitalWrite(dir4, HIGH);

  analogWrite(pwm1, 40);
  analogWrite(pwm2, 0);
  analogWrite(pwm3, 40);
  analogWrite(pwm4, 0);

}
float moveLeftBW_d()           //diagonal left back movement

{
  Serial.println("sashi buri2");
  digitalWrite(dir1, LOW);
  digitalWrite(dir2, LOW);
  digitalWrite(dir3, LOW);
  digitalWrite(dir4, LOW);

  analogWrite(pwm1, 0);
  analogWrite(pwm2, 40);
  analogWrite(pwm3, 0);
  analogWrite(pwm4, 40);
}
float moveRightBW_d()             //diagonal right back movement
{
  Serial.println("sashi buri3");
  digitalWrite(dir1, LOW);
  digitalWrite(dir2, LOW);
  digitalWrite(dir3, LOW);
  digitalWrite(dir4, LOW);

  analogWrite(pwm1, 40);
  analogWrite(pwm2, 0);
  analogWrite(pwm3, 40);
  analogWrite(pwm4, 0);

}

int ultdist(int t, int e)
{ int distance;
  long duration;
  digitalWrite(t, LOW);       //distance calc
  delayMicroseconds(2);
  digitalWrite(t, HIGH);
  delayMicroseconds(10);
  digitalWrite(t, LOW);
  duration = pulseIn(e, HIGH);
  distance = (duration / 2 ) * 0.0343;
  return distance;
}

int PID(float x, double t, int y)
{
  input = x;
  err = input - y;

  //calculate p term
  p = kp * err;
  //calculate i term
  d = kd * (last_err - err) / t;

  last_err = err;
  //calculate i term
  i_err += err;
  i = ki * i_err;

  float output = p + d - i;      //output
  return output;

}

/*void throwing()
  {
  Serial.println("up");
  digitalWrite(dir9, LOW);
  analogWrite(pwm9, 255);

  if (pulses > 80)
  {
    tflag = 2;
  }
  }
  void back()
  {
  Serial.println("values are coming");

  //Serial.println("val coming");
  //pwm1 = encoderValue - 60;

  /*if (pulses > 40) {
    digitalWrite(dir9, HIGH);
    analogWrite(pwm9, 40);
  }
  /*if (pulses <= 40)
  {
    digitalWrite(dir9, HIGH);
    analogWrite(pwm9, 25);
  }*/

/*if (pulses < 10)
  {
    tflag = 0;
  }
  }*/


void A_CHANGE() {
  //Interrupt function to read the x2 pulses of the encoder.

  if ( digitalRead(ENC_INB) == 0 )
  {
    if ( digitalRead(ENC_INA) == 0 )
    {
      // A fell, B is low
      pulses--; // Moving forward
    } else
    {
      // A rose, B is high
      pulses++; // Moving reverse
    }
  }
  else {
    if ( digitalRead(ENC_INA) == 0 ) {
      pulses++;
      //Serial.println(pulses);// Moving reverse
    } else {
      // A rose, B is low
      pulses--;
      // Serial.println(pulses);// Moving forward
    }
  }
}
