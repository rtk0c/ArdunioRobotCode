#include <Servo.h>

// Pins
const int RIGHT_ENABLE = 9;
const int RIGHT_IN1 = 7;
const int RIGHT_IN2 = 8;
const int LEFT_ENABLE = 0;
const int LEFT_IN1 = 1;
const int LEFT_IN2 = 2;
const int SERVO_SIGNAL = 5;
const int SENSOR_TRIG = 2;
const int SENSOR_ECHO = 3;

Servo sensor;

void setup() {
  Serial.begin(9600);
  pinMode(RIGHT_ENABLE, OUTPUT);
  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);
  pinMode(LEFT_ENABLE, OUTPUT);
  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);
  pinMode(SERVO_SIGNAL, OUTPUT);
  pinMode(SENSOR_TRIG, OUTPUT);
  pinMode(SENSOR_ECHO, INPUT);
  sensor.attach(SERVO_SIGNAL);
}

int pos = 0;
void loop() {
  triggerSens();
  Serial.print(getDistCM());
  Serial.println(" cm");

  sensor.write(pos);
  pos += 5;
  if (pos >= 180) {
    pos = 0;
  }

  delay(100);
}

void triggerSens() {
  digitalWrite(SENSOR_TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(SENSOR_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(SENSOR_TRIG, LOW);
}

double getDistCM() {
  unsigned long duration = pulseIn(SENSOR_ECHO, HIGH);
  return duration / 2 / 29.1;
}
