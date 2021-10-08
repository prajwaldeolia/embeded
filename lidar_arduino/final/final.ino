int n
void setup()
  {
    Serial1.begin(57600);
    Serial.begin(57600);
    Serial.flush();
  }

void loop()
  {
       
  }
  
  
  //Functions....
  
int values()
  {
    int product=10;
    int sum=0;
    int number;
    while (Serial1.available())                      //try if in place of while;
    {
      for(int i=0; i<2; i++)
        { int n1 = 0;
          number = Serial1.read();
          n1 = number - 48;
          sum += (n1 * product);
          product /= 10;
          Serial.println(sum);
          sum=0;
          product=10;
          return number;
        }
    }
  }

