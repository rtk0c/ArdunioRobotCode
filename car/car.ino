#include <Servo.h>

class RangedInt {
  public:
    int num;
    int llimit;
    int ulimit;
    int change;
    bool state;

    RangedInt(int start, int change, int llim, int ulim) {
      this->num = start;
      this->llimit = min(llim, ulim);
      this->ulimit = max(llim, ulim);
      this->change = change;
      this->state = true; // Increasing
    }

    void next() {
      bool state = this->state;
      if (state) {
        // Increasing
        this->num += this->change;
      } else {
        // Decreasing
        this->num -= this->change;
      }

      int num = this->num;
      int llim = this->llimit;
      int ulim = this->ulimit;
      if (num < llim) {
        this->num = llim;
        this->state = !state;
      } else if (num > ulim) {
        this->num = ulim;
        this->state = !state;
      }
    }

    void middle() {
      this->num = (this->ulimit - this->llimit) / 2;
    }
    void bottom() {
      this->num = this->llimit;
    }
    void top() {
      this->num = this->ulimit;
    }
    void fractionIn(double fr) {
      unsigned int range = this->ulimit - this->llimit;
      this->num = this->llimit + range*fr;
    }
};

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

const unsigned int SENSOR_MIDDLE_FRONT = 60 /* degrees */;
const unsigned int ROTATE_90_DEG_MS = 330;

Servo sensor;
RangedInt pos(0, 5, 0, 179);

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

//  sensor.write(0);
//  delay(500);
//  sensor.write(180);
//  delay(500);
//  pos.middle();
//  Serial.println(89);
//  sensor.write(pos.num);
  sensor.write(SENSOR_MIDDLE_FRONT);

  turnLeft(90);
  delay(1000);
  turnRight(180);
}

const double kp = 0.5;
const int target = 20;

bool running = true;
void loop() {
  return;
  if(!running) {
    return;
  }
//  pos.middle();
//  sensor.write(pos.num);

  triggerSensor();
  double dist = getDistCM();
  double error = target - dist;
  // Going forward is CCW
  moveBoth(constrain(error * kp, -1, 1));
//  moveBoth(error < 0 ? -1 : 1);

  if(abs(error) < 1) {
    stopMotors();
    running = false;
  }

//  Serial.println("ff");
//  Serial.println(error * kp); 
//  Serial.println(constrain(error * kp, -1, 1));

  delay(10);
}

void triggerSensor() {
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
