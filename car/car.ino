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
};

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

RangedInt pos(0, 5, 0, 179);
void loop() {
  triggerSens();
  Serial.print(getDistCM());
  Serial.println(" cm");

  sensor.write(pos.num);
  pos.next(); `

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
