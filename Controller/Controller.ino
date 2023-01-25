#include <Wire.h>

double thr = 300.0;
int n = 0;

double x = 0.5;
double y = 0.5;
bool gotcha = false;

double gradient[3];

//double testF(double x1, double y1) {
//  return (1-x1)*(1-x1) + 100*(y1-x1*x1)*(y1-x1*x1);
//}

double testF(double x1, double y1) {
  double value = 0.0;
  DAC(int(x1*1023),'A',0);
  DAC(int(y1*1023),'B',2);
  delay(10);
  value += analogRead(A3);
  delay(10);
  value += analogRead(A3);
  delay(10);
  value += analogRead(A3);
  delay(10);
  value += analogRead(A3);
  delay(10);
  value += analogRead(A3);
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
  int iters = 1000;
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
  double currentStep = 5e-2;
  Serial.println("-------------SD-------------");
  while (true) {
    NumericalGradient(currentStep);

    double f0 = testF(x, y);

    Serial.print(x);
    Serial.print("\t");
    Serial.print(y);
    Serial.print("\t");
    Serial.print(f0);
    Serial.print("\t");
    Serial.print(testF(x+currentStep, y));
    Serial.print("\t");
    Serial.print(testF(x, y+currentStep));
    Serial.print("\t");
    Serial.print(testF(x+gradient[0]*currentStep, y+gradient[1]*currentStep));
    Serial.print("\t");
    Serial.print(gradient[1]);
    Serial.print("\t");
    Serial.println(gradient[0]);


    // Change currentStep
    if (n>5) {
      double f1 = testF(x + gradient[0]*currentStep*2, y + gradient[1]*currentStep*2);
      double f2 = testF(x + gradient[0]*currentStep*0.5, y + gradient[1]*currentStep*0.5);

      if (f0<0.33*thr) {
        gotcha = false;
        break;
      }

      if (f1>f0 && f1>f2) {
        currentStep = min(2*currentStep,1e-1);
      }
      if (f2>f0 && f2>=f1) {
        currentStep = max(0.5*currentStep,1e-4);
      }
      n=0;
    }
    n+=1;

    //Serial.print("\t");
    //Serial.print(gradient[0]);
    //Serial.print("\t");
    //Serial.println(gradient[1]);
    
    x += gradient[0]*currentStep;
    y += gradient[1]*currentStep;

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
  double norm = sqrt(gradient[0]*gradient[0]+gradient[1]*gradient[1]+1e-5);
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
