#include <Wire.h>

void DAC (int v, char channel, byte mode) {
  byte b=0;
  Wire.beginTransmission(0x4C); //0x48 or 0x4C
    // A3 A2 L1 L0 X Sel1 Sel0 PD0
  channel-='A';
  b|=(channel<<1);
  b|=(mode<<4);
  Wire.write(b);
    // D9 D8 D7 D6 D5 D4 D3 D2
  Wire.write(v>>2);
    // D1 D0 X X X X X X
  Wire.write(v<<6);
  Wire.endTransmission(); 
};

void setup()
{
  Wire.begin();
  Wire.setClock(400000);// 400000
  Serial.begin(9600);  
  while (!Serial);
  Serial.println("Connected");
}

void loop()
{
  DAC(int(0),'A',0);
  DAC(int(0),'B',2);
  delay(5);
  DAC(int(0),'A',0);
  DAC(int(1023),'B',2);
  delay(5);
  DAC(int(1023),'A',0);
  DAC(int(1023),'B',2);
  delay(5);
  DAC(int(1023),'A',0);
  DAC(int(0),'B',2);
  delay(5);
}
