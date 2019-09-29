#include <Servo.h>

#define CLAW_SERVO_OPEN 90
#define CLAW_SERVO_CLOSE 180

#define BUTTON_PIN 7
#define X_ROT_POT_PIN A0
#define Y_ROT_POT_PIN A1
#define UPDOWN_POT_PIN A2

Servo clawServo;
Servo upDownServo;
Servo xRotationServo;
Servo yRotationServo;

bool clawOpened;

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
    value = map(analogRead(UPDOWN_POT_PIN), 0, 1023, 0, 180);
    upDownServo.write(value);
}
