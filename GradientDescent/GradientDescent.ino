

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

int v=0;
//int v1=0;
//int v2=0;
void setup()
{
  Wire.begin();
  Wire.setClock(400000);// 400000
  Serial.begin(9600);
  delay(500);
  
  while (!Serial);
  Serial.println("Connected");
}

void loop()
{

/*
DAC(700,'A',1);
DAC(400,'A',1);
*/

/*  
DAC(600,'A',1);
DAC(600,'B',1);
DAC(300,'A',1);
DAC(300,'B',1);
*/

/*
DAC(000,'A',0);
DAC(500,'B',2);
DAC(500,'A',0);
DAC(000,'B',2);
*/

/*
DAC(v,'A',0);
DAC(v,'B',2);
v++; if (v>1023) {v=0;};
*/
v=v+20;
v++; if (v>1023) {v=0;};
DAC(int((sin(v*3.14159*2/1023)+1)*1023/2),'A',0);
DAC(int((cos(3*v*3.14159*2/1023)+1)*1023/2),'B',2);
}
