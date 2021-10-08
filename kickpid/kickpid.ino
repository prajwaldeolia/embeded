#define ENC_INA 19
#define ENC_INB 18
#define dir9 3
#define pwm9 2
float input,output;
float p,i,d;
float err,i_err,last_err;
int threshhold;
double kp=0.8;                                    //PID constant values for normal movement
double kd=0.4;
double ki=0.01;

double dt;
unsigned long currentmillis,prevmillis=0,elapsedtime;
int pulses=0;
int flag=0,a=40,b=-140;        //a=front set point
                               //b=back set point
int r=2,u,v;
void setup()
{
  attachInterrupt(digitalPinToInterrupt(ENC_INA), A_CHANGE, RISING);
  pinMode(ENC_INA, INPUT);
  pinMode(ENC_INB, INPUT);
  pinMode(pwm9, OUTPUT);
  pinMode(dir9, OUTPUT); 
  Serial.begin(9600); 
}

void loop()
{
  currentmillis=millis();
  elapsedtime=(currentmillis-prevmillis);
  dt=elapsedtime;
  Serial.println(pulses);
  u=PID(pulses,dt,b);
  v=PID(pulses,dt,a);
  
  if(pulses>=b+r)
  {
    digitalWrite(dir9,LOW);
    analogWrite(pwm9,15+u);
    
  }
  else if(pulses<b-r) 
  {     
    digitalWrite(dir9,HIGH);
    analogWrite(pwm9,30+u);
  }
  else 
  { 
    flag=1;
  }
  if(flag==1)
  { if(pulses<=a-r)
    {digitalWrite(dir9,HIGH);
     analogWrite(pwm9,200);
    }
    else if(pulses>a+r)
    {
    digitalWrite(dir9,LOW);
    analogWrite(pwm9,30+v);  
    }
    else
    {
      flag=2;
    }
  }
  if(flag==2)
  {
    
    analogWrite(pwm9,0); 
  }
 prevmillis=currentmillis;
delay(30);
}




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
  else if(digitalRead(ENC_INB) == 1) {
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
int PID(float x,double t,int y)
{
  input=x;
    err=input-y;

  //calculate p term
  p=kp*err;
    //calculate i term
  d=kd*(last_err-err)/t; 
  
  last_err=err;
                     //calculate i term
  i_err+=err;
  i=ki*i_err;
   
 float output=p+d-i;            //output 
 return output; 
  
}
