void setup()
{
  Serial.begin(9600);
  delay(500);
  
  while (!Serial);
  Serial.println("Connected");
}

void loop()
{
  Serial.println(analogRead(A3));
  //BlindSearch();
  //DAC(int((sin(v*3.14159*2/1023)+1)*1023/2),'A',0);
  //DAC(int((cos(3*v*3.14159*2/1023)+1)*1023/2),'B',2);
}
