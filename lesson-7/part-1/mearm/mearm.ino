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
    // Устанавливаем пины для потенциометров и кнопки в режим чтения
    pinMode(BUTTON_PIN, INPUT);
    pinMode(X_ROT_POT_PIN, INPUT);
    pinMode(Y_ROT_POT_PIN, INPUT);

    // Установка управляющих пинов для серв
    clawServo.attach(8);
    upDownServo.attach(9);
    xRotationServo.attach(10);
    yRotationServo.attach(11);

    // Установка серв в их начальные позиции
    clawServo.write(CLAW_SERVO_OPEN);
    upDownServo.write(UPDOWN_SERVO_UP);
    clawOpened = true;
    upDown = true;
}

void loop() {
    int data[4];
    readData(data);

    // Считываем состояние кнопки и поворачиваем серво
    if (data[0] == 1) {
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
    if (data[3] > 0) {
        if (!upDown) {
            upDownServo.write(UPDOWN_SERVO_UP);
            upDown = true;
        }
    } else if(data[3] < 0) {
        if (upDown) {
            upDownServo.write(UPDOWN_SERVO_DOWN);
            upDown = false;
        }
    }
}

/*
 * Функция для чтения данных с потенциометров и кнопок
 * outputData[] - целочисленный массив, в который сохраняются полученные значения
 * outputData[0] - статус кнопки
 * outputData[1] и outputData[2] - статус потенциометров
 * outputData[3] - TODO
 */
void readData(int outputData[]) {
    outputData[0] = digitalRead(BUTTON_PIN);
    outputData[1] = map(analogRead(X_ROT_POT_PIN), 0, 1023, -100, 100);
    outputData[2] = map(analogRead(Y_ROT_POT_PIN), 0, 1023, -100, 100);
    outputData[3] = 1; // временно 
}
