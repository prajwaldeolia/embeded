int analogPin=A8;
float val=0,d=0;

void setup() {
  pinMode(analogPin,INPUT);
  //pinMode(e,OUTPUT);
  Serial.begin(9600);          
}

void loop() {
 val = analogRead(analogPin);
 //d=map(val,0,676,0,9);
  Serial.print("val:");
 Serial.println(val);
 //Serial.print("d:");
 //Serial.println(d);
 /*if(d>1&&d<6)
 {
  digitalWrite(e,HIGH);
  }
  else
  {
   digitalWrite(e,LOW);
  }*/
  delay(10);
}
