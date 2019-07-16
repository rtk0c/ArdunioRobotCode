const int PORT_ENABLE = 9;
const int PORT_IN_1 = 7;
const int PORT_IN_2 = 8;

const bool CCW = true;
const bool CW = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(PORT_ENABLE, OUTPUT);
  pinMode(PORT_IN_1, OUTPUT);
  pinMode(PORT_IN_2, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  spinDirection(CCW);
  analogWrite(PORT_ENABLE, 200);

  delay(1000);

  spinDirection(CW);
  analogWrite(PORT_ENABLE, LOW);

  delay(1000);
}

void spinDirection(bool dir) {
  digitalWrite(PORT_IN_1, dir ? LOW : HIGH);
  digitalWrite(PORT_IN_2, dir ? HIGH : LOW);
}
