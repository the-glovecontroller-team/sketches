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

void setup() {
    // Устанавливаем пины для потенциометров и кнопки в режим чтения
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(X_ROT_POT_PIN, INPUT);
    pinMode(Y_ROT_POT_PIN, INPUT);
    pinMode(UPDOWN_POT_PIN, INPUT);

    // Установка управляющих пинов для серв
    clawServo.attach(8);
    upDownServo.attach(9);
    xRotationServo.attach(10);
    yRotationServo.attach(11);

    // Установка серв в их начальные позиции
    clawServo.write(CLAW_SERVO_OPEN);
    clawOpened = true;
}

void loop() {
    int data[4];
    readData(data);

    // Считываем состояние кнопки и поворачиваем серво
    if (data[0] == 1) {
        clawServo.write(CLAW_SERVO_CLOSE);
    } else {
        clawServo.write(CLAW_SERVO_OPEN);
    }

    // Читаем данные о повороте вокруг оси X в диапазоне (-100, 100) и переводим их в диапазон (0, 180)
    int value = data[1];
    int adaptedValue = map(value, -100, 100, 0, 180);
    // Поворачиваем серво
    xRotationServo.write(adaptedValue);

    // Читаем данные о повороте вокруг оси Y в диапазоне (-100, 100) и переводим их в диапазон (0, 180)
    value = data[2];
    adaptedValue = map(value, -100, 100, 0, 180);
    // Поворачиваем серво
    yRotationServo.write(adaptedValue);

    // Читаем данные о движении вверх/вниз и поворачиваем серво
    value = data[3];
    adaptedValue = map(value, -100, 100, 0, 180);
    // Поворачиваем серво
    upDownServo.write(adaptedValue);
}

/*
 * Функция для чтения данных с потенциометров и кнопок
 * outputData[] - целочисленный массив, в который сохраняются полученные значения
 * outputData[0] - статус кнопки
 * outputData[1], outputData[2] и outputData[3] - статус потенциометров
 */
void readData(int outputData[]) {
    outputData[0] = digitalRead(BUTTON_PIN);
    outputData[1] = map(analogRead(X_ROT_POT_PIN), 0, 1023, -100, 100);
    outputData[2] = map(analogRead(Y_ROT_POT_PIN), 0, 1023, -100, 100);
    outputData[3] = map(analogRead(UPDOWN_POT_PIN), 0, 1023, -100, 100);
}
