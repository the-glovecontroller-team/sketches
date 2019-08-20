#include <Servo.h>

#define CLAW_SERVO_OPEN 90
#define CLAW_SERVO_CLOSE 180
#define UPDOWN_SERVO_UP 170
#define UPDOWN_SERVO_DOWN 10

#define BUTTON_PIN 7
#define X_ROT_POT_PIN A0
#define Y_ROT_POT_PIN A1

Servo clawServo;
Servo upDownServo;
Servo xRotationServo;
Servo yRotationServo;

bool clawOpened;
bool upDown; // true -> вверх(up); false -> вниз(down)

void setup() {
    // Установка управляющих пинов для серв
    clawServo.attach(8);
    upDownServo.attach(9);
    xRotationServo.attach(10);
    yRotationServo.attach(11);
}

void loop() {
    // Считываем состояние кнопки и поворачиваем серво
    if (digitalRead(BUTTON_PIN) == 1) {
        // Если клешня открыта - закрываем, если нет - ничего не делаем
        if (clawOpened) {
            clawServo.write(CLAW_SERVO_CLOSE);
            clawOpened = false;
        }
    } else {
        if (!clawOpened) {
            clawServo.write(CLAW_SERVO_OPEN);
            clawOpened = true;
        }
    }

    // Читаем данные о повороте вокруг оси X и поворачиваем серво
    int value = map(analogRead(X_ROT_POT_PIN), 0, 1023, 0, 180);
    xRotationServo.write(value);

    // Читаем данные о повороте вокруг оси Y и поворачиваем серво
    value = map(analogRead(Y_ROT_POT_PIN), 0, 1023, 0, 180);
    yRotationServo.write(value);

    // Читаем данные о движении вверх/вниз и поворачиваем серво
    int dir = 1; // Временно запишем туда 1
    if (dir > 0) {
        if (!upDown) {
            upDownServo.write(UPDOWN_SERVO_UP);
            upDown = true;
        }
    } else if(dir < 0) {
        if (upDown) {
            upDownServo.write(UPDOWN_SERVO_DOWN);
            upDown = false;
        }
    }
}
