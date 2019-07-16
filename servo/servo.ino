#include <Servo.h>

const int PORT = 9;

// Red = positive
// Brown = ground
// Orange = singal
Servo servo;
int pos = 0;

void setup() {
  // put your setup code here, to run once:
  servo.attach(PORT);
}

void loop() {
  // put your main code here, to run repeatedly:
  servo.write(pos);
  ++pos;
  if(pos >= 180) {
    pos = 0;
  }
  delay(15);
}
