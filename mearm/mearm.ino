#include <Servo.h>

Servo servoArm;
Servo servoY;
Servo servoRotY;
int currentServo = 0;

void setup() {
  servoArm.attach(7);
  servoY.attach(6);
  servoRotY.attach(5);
  
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    // Формат команд: A:num:num:\n:
    String input = Serial.readStringUntil(':');
    if (input == "A") {
      servoArm.write(180);
    } else if (input == "N") {
      servoArm.write(90);
    } else if (input == "\n") {
      currentServo = 0;
    } else {
      switch(currentServo++) {
        case 0:
          servoY.write(input.toInt());
          break;
        case 1:
          servoRotY.write(input.toInt());
          break;
        default:
          Serial.print(input);
          // If you're here, something is wrong
          break;
      }
    } 
  }
}
