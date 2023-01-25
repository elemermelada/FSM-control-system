#include <Wire.h>

float thr = 150.0;
float n = 0;

float x = 0.5;
float y = 0.5;
bool gotcha = false;

float gradient[2];

//float testF(float x1, float y1) {
//  return (1-x1)*(1-x1) + 100*(y1-x1*x1)*(y1-x1*x1);
//}

float testF(float x1, float y1) {
  //Serial.println("testF");
  DAC(int(x1*1023),'A',0);
  DAC(int(y1*1023),'B',2);
  delay(5);
  float value = analogRead(A3);
  DAC(int(x*1023),'A',0);
  DAC(int(y*1023),'B',2);
  //Serial.println("testF - out");
  return value;
  //return 1023*pow(pow(2.718,-(pow(x1-0.90,2)+pow(y1-0.1,2))),20);
}

void DAC (int v, char channel, byte mode) {
  //Serial.print("Sent me to: ");
  //Serial.println(v);
  //Serial.print("In channel: ");
  //Serial.println(channel);
  byte b=0;
  Wire.beginTransmission(0x4C);
  channel-='A';
  b|=(channel<<1);
  b|=(mode<<4);
  Wire.write(b);
  Wire.write(v>>2);
  Wire.write(v<<6);
  Wire.endTransmission(); 
};

// TODO - corner search
void SpiralSearch() {
  int iters = 1000;
  int loopStep = 10;
  float r = 0;
  float th = 0;
  
Serial.println("-------------Spiral-------------");
  for(int i = 0;i < iters;i++) {
    
    Serial.print(r);
    Serial.print("\t");
    Serial.print(y);
    Serial.print("\t");
    Serial.println(analogRead(A3));

    r = 0.5*i/iters;
    th = 2.0*3.14159*loopStep*i/iters;

    x = 0.5 + r*cos(th);
    y = 0.5 + r*sin(th);

    DAC(int(x*1023),'A',0);
    DAC(int(y*1023),'B',2);

    if (testF(x,y)>thr) {
      gotcha = true;
      break;
    }
  }
}

void SteepestDescent() {
  float currentStep = 1e-2;
  Serial.println("-------------SD-------------");
  while (true) {
    NumericalGradient(currentStep);

    //Serial.print(x);
    //Serial.print("\t");
    //Serial.println(y);
    //Serial.print(gradient[0]);
    //Serial.print("\t");
    //Serial.println(gradient[1]);

    float f0 = testF(x, y);

    Serial.print(x);
    Serial.print("\t");
    Serial.print(y);
    Serial.print("\t");
    Serial.print(f0);
    Serial.print("\t");
    Serial.println(currentStep*10000.0);

    // Change currentStep
    if (false) {
      float f1 = testF(x + gradient[0]*currentStep*2, y + gradient[1]*currentStep*2);
      float f2 = testF(x + gradient[0]*currentStep*0.5, y + gradient[1]*currentStep*0.5);

      //if (f0<thr-100) {
      //  gotcha = false;
      //  break;
      //}

      if (f1>f0 && f1>f2) {
        currentStep = 2*currentStep;
      }
      if (f2>f0 && f2>=f1) {
        currentStep = 0.5*currentStep;
      }
      n=0;
    }
    //n+=1;

    //Serial.println(gradient[0]/sqrt(gradient[0]*gradient[0]+gradient[1]*gradient[1]+0.01));
    //Serial.println(gradient[1]/sqrt(gradient[0]*gradient[0]+gradient[1]*gradient[1]+0.01));
    //Serial.print("\t");
    //Serial.println(gradient[1]*currentStep);

    x += gradient[0]/sqrt(gradient[0]*gradient[0]+gradient[1]*gradient[1]+0.01)*currentStep;
    y += gradient[1]/sqrt(gradient[0]*gradient[0]+gradient[1]*gradient[1]+0.01)*currentStep;

    DAC(int(x*1023),'A',0);
    DAC(int(y*1023),'B',2);
  }
}

void NumericalGradient(float delta) {
  // Derivate respect x
  float f1 = testF(x+delta,y);
  float f2 = testF(x-delta,y);
  gradient[0] = 0.5*(f1-f2)/delta;

  // Derivate respect y
  f1 = testF(x,y+delta);
  f2 = testF(x,y-delta);
  gradient[1] = 0.5*(f1-f2)/delta;
}

int v=0;
void setup()
{
  Wire.begin();
  Wire.setClock(400000);// 400000
  Serial.begin(9600);
  while (true) {
    delay(500);
    DAC(int(0),'A',0);
    delay(500);
    DAC(int(1023),'A',0);
    delay(500);
    DAC(int(0),'B',2);
    delay(500);
    DAC(int(1023),'B',2);
  }
  
  while (!Serial);
  Serial.println("Connected");
}

void loop()
{
  v=v+20;
  v++; if (v>1023) {v=0;};
  //if (analogRead(A3)<10) {
  //  gotcha = false;
  //}
  if (!gotcha) {
    SpiralSearch();
  }
  SteepestDescent();
  //BlindSearch();
  //DAC(int((sin(v*3.14159*2/1023)+1)*1023/2),'A',0);
  //DAC(int((cos(3*v*3.14159*2/1023)+1)*1023/2),'B',2);
}
