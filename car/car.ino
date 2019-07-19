#include <Servo.h>

// Pins
const unsigned int RIGHT_ENABLE = 9;
const unsigned int RIGHT_IN1 = 7;
const unsigned int RIGHT_IN2 = 8;
const unsigned int LEFT_ENABLE = 10;
const unsigned int LEFT_IN1 = 11;
const unsigned int LEFT_IN2 = 12;
const unsigned int SERVO_SIGNAL = 5;
const unsigned int SENSOR_TRIG = 3;
const unsigned int SENSOR_ECHO = 2;

const unsigned int SENSOR_MIDDLE_FRONT = 80 /* degrees */;
const unsigned int SENSOR_RIGHT = 0;
const unsigned int SENSOR_LEFT = 160;
const unsigned int ROTATE_90_DEG_MS = 330;

const double kp = 0.5;

enum State { forward,  checkLeft,  turningLeft,  checkRight,  turningRight,  backward };

State state = forward;
bool running = true;

Servo servo;

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
  servo.attach(SERVO_SIGNAL);
  servo.write(SENSOR_MIDDLE_FRONT);
}

void loop() {
  if (!running) {
    return;
  }

  double dist = -1;
  unsigned int target = -1;
  double error = -1;

  switch (state) {
    case forward: {
        // Car is either starting to move or is moving
        servo.write(SENSOR_MIDDLE_FRONT);

        dist = getDistCM();
        target = 20;
        error = target - dist;

        if (abs(error) < 1) {
          stopMotors();
          delay(1000); // Stablize car's motion
          state = checkLeft;
        } else {
          // TODO fix motor not working under less power
          /*double spd = mapDb(constrain(error * kp, -10, 10), -10, 10, -1, 1);
            moveBoth(spd);*/
          moveBoth(constrain(error * kp, -1, 1));
        }
        break;
      }
    // Car is completely stopped
    case backward: {
        servo.write(SENSOR_MIDDLE_FRONT);

        dist = getDistCM();
        target = 20;
        error = target - dist;

        moveBoth(1);
        delay(2000);
        stopMotors();
        running = false;
        break;
      }
    // Car is completely stopped
    case checkLeft: {
        servo.write(SENSOR_LEFT);
        delay(1000); // Give time for the servo to turn

        dist = getDistCM();
        target = 30;
        error = target - dist;

        state = dist < target ? checkRight : turningLeft;
        break;
      }
    // Car is completely stopped
    case turningLeft: {
        servo.write(SENSOR_MIDDLE_FRONT);

        dist = getDistCM();
        target = 20;
        error = target - dist;

        turnLeft(90);
        delay(100); // Stablize car's motion
        state = forward;
        break;
      }
    // Car is completely stopped
    case checkRight: {
        servo.write(SENSOR_RIGHT);
        delay(1000); // Give time for the servo to turn

        dist = getDistCM();
        target = 30;
        error = target - dist;

        state = dist < target ? backward : turningRight;
        break;
      }
    case turningRight: {
        servo.write(SENSOR_MIDDLE_FRONT);

        dist = getDistCM();
        target = 20;
        error = target - dist;

        turnRight(90);
        delay(100); // Stablize car's motion
        state = forward;
        break;
      }
  }

  Serial.print("dist=");
  Serial.print(dist);
  Serial.print(" erorr=");
  Serial.print(error);
  Serial.println(" | state=forward");
}

void triggerSensor() {
  digitalWrite(SENSOR_TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(SENSOR_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(SENSOR_TRIG, LOW);
}

double getDistCM() {
  triggerSensor();
  unsigned long duration = pulseIn(SENSOR_ECHO, HIGH);
  return duration / 2 / 29.1;
}

// Positive = CW
// Negative = CCW
void moveBoth(double speed) {
  moveLeft(speed);
  moveRight(speed);
}
void moveLeft(double speed) {
  mvMot(LEFT_ENABLE, LEFT_IN1, LEFT_IN2, speed);
}
void moveRight(double speed) {
  mvMot(RIGHT_ENABLE, RIGHT_IN1, RIGHT_IN2, speed);
}
void mvMot(unsigned int pinEn, unsigned int pin1, unsigned int pin2, double speed) {
  if (speed == 0) {
    analogWrite(pinEn, 0);
  } else {
    digitalWrite(pin1, speed < 0 ? HIGH : LOW);
    digitalWrite(pin2, speed < 0 ? LOW : HIGH);
    analogWrite(pinEn, max(abs(speed), 0.5) * 255);
  }
}

void stopMotors() {
  moveLeft(0);
  moveRight(0);
}

void turn(double rot) {
  moveLeft(rot);
  moveRight(-rot);
}
void turnInternal(unsigned int deg, double rot) {
  double amount90Deg = (double) deg / 90;
  unsigned int delayMs = amount90Deg * ROTATE_90_DEG_MS;
  turn(rot);
  delay(delayMs);
  stopMotors();
}
void turnLeft(unsigned int deg) {
  turnInternal(deg, 1);
}
void turnRight(unsigned int deg) {
  turnInternal(deg, -1);
}
