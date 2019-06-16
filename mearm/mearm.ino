#include <Servo.h>

Servo servoArm;
Servo servoY;
Servo servoRotY;
int currentServo = 0;

void setup() {
  servoArm.attach(7);
  servoY.attach(6);
  servoRotY.attach(8);
  
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    // Формат команд: A:N:N:N:num:num:num:\n:
    currentServo++;
    String input = Serial.readStringUntil(':');

    if (input == "A") {
      if (currentServo == 1) {
        servoArm.write(180);
      }
    } else if (input == "N") {
      if (currentServo == 1) {
        servoArm.write(90);
      }
    } else if (input == "\n") {
      currentServo = 0;
    } else {
      if (currentServo == 5) {
          int value = input.toInt();
          value = value > 100 ? 100 : value;
          value = value < -100 ? -100 : value;
          value += 100;
          value = value * 180 / 200;
          servoY.write(value);
      } else if (currentServo == 6) {
          int value = input.toInt();
          value = value > 100 ? 100 : value;
          value = value < -100 ? -100 : value;
          value += 100;
          value = value * 180 / 200;
          servoRotY.write(value);
      }
    } 
  }
}
