#include <Servo.h>

#define BUTTON_PIN 7
#define X_ROT_POT_PIN A0
#define Y_ROT_POT_PIN A1
#define UPDOWN_POT_PIN A2

Servo clawServo;
Servo upDownServo;
Servo xRotationServo;
Servo yRotationServo;

void setup() {
    // Установка управляющих пинов для серв
    clawServo.attach(8);
    upDownServo.attach(9);
    xRotationServo.attach(10);
    yRotationServo.attach(11);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
    // Считываем состояние кнопки и поворачиваем серво
    if (digitalRead(BUTTON_PIN) == 1) {
        clawServo.write(180);
    } else {
        clawServo.write(90);
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
