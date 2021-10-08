#define rx 19
#define tx 18

void setup() 
{
  // Begin serial communication with baudrate 9600
  Serial1.begin(57600);
  Serial.begin(57600);
}
void loop() 
{

  while (Serial1.available() <= 0);  // Wait for data to be available
  positionVal = Serial1.read(); 
}


char buffer[10] = {0,0,0,0,0,0,0,0,0,0};
void setup(){
    Serial.begin(9600);
}
void loop(){
    Serial.println(Serial.readBytes(buffer,5));
}
