#include <Wire.h>

double thr = 100.0;
int dly = 1;

double x = 0.5;
double y = 0.5;

bool gotcha = false;

double gradient[3];

//double testF(double x1, double y1) {
//  return (1-x1)*(1-x1) + 100*(y1-x1*x1)*(y1-x1*x1);
//}

float testF(double x1, double y1) {
  float value = 0.0;
  delay(dly);
  DAC(int(x1*1023),'A',0);
  DAC(int(y1*1023),'B',2);
  delay(dly);
  value += analogRead(A3);
  delay(dly);
  value += analogRead(A3);
  delay(dly);
  value += analogRead(A3);
  delay(dly);
  value += analogRead(A3);
  //delay(dly);
  //value += analogRead(A3);
  return value;
  //return 1023*pow(pow(2.718,-(pow(x1-0.90,2)+pow(y1-0.1,2))),20);
}

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

// TODO - corner search
void SpiralSearch() {
  int iters = 500;
  int loopStep = 10;
  double r = 0;
  double th = 0;
  
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
  double currentStep = 1e-2;
  Serial.println("-------------SD-------------");
  while (true) {
    NumericalGradient(currentStep);

    double f0 = testF(x, y);

    Serial.print(x);
    Serial.print("\t");
    Serial.print(y);
    Serial.print("\t\t");
    Serial.print(f0);
    Serial.print("\t\t");
    //Serial.print(testF(x+currentStep, y));
    //Serial.print("\t");
    //Serial.print(testF(x, y+currentStep));
    //Serial.print("\t");
    //Serial.print(testF(x+gradient[0]*currentStep, y+gradient[1]*currentStep));
    //Serial.print("\t");
    Serial.print(gradient[0]);
    Serial.print("\t");
    Serial.print(gradient[1]);
    Serial.print("\t");
    Serial.println(gradient[2]);

    // Change currentStep
    //currentStep = 2e-3*(10.0-9.0*pow(1.05,gradient[2]));

    // Return to spiral search
    if (f0<0.33*thr) {
      gotcha = false;
      break;
    }
    
    x += gradient[0]*currentStep;
    y += gradient[1]*currentStep;

    x = max(x,0.01);
    x = min(x,0.99);
    y = max(y,0.01);
    y = min(y,0.99);    

    DAC(int(x*1023),'A',0);
    DAC(int(y*1023),'B',2);
  }
}

void NumericalGradient(double delta) {
  // Derivate respect x
  double f1 = testF(x+delta,y);
  double f2 = testF(x-delta,y);
  gradient[0] = 0.5*(f1-f2)/delta;

  // Derivate respect y
  f1 = testF(x,y+delta);
  f2 = testF(x,y-delta);
  gradient[1] = 0.5*(f1-f2)/delta;

  // Normalize
  double norm = sqrt(gradient[0]*gradient[0]+gradient[1]*gradient[1]+1e-7);
  gradient[0] /= norm;
  gradient[1] /= norm;
  gradient[2] = norm;
}

int v=0;
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
  v=v+20;
  v++; if (v>1023) {v=0;};
  if (!gotcha) {
    SpiralSearch();
  }
  SteepestDescent();
}
