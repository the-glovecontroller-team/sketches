#include <Arduino.h>
#include "SmoothGyro.h"

#define FINGER_1_PIN 8
#define FINGER_2_PIN 9
#define FINGER_3_PIN 10
#define FINGER_4_PIN 11

SmoothGyro* gyro;

int16_t prevXPos;
int16_t prevYPos;
int16_t prevZPos;

int32_t dXPos;
int32_t dYPos;
int32_t dZPos;

int yDirection;

int getDirection(int prevDirection, int16_t delta);

/*
   Подготовка устройства
*/
void setup() {
    // Устанавливаем пины кнопок в режим ввода
    pinMode(FINGER_1_PIN, INPUT);
    pinMode(FINGER_2_PIN, INPUT);
    pinMode(FINGER_3_PIN, INPUT);
    pinMode(FINGER_4_PIN, INPUT);

    Serial.begin(9600);

    // Инициализируем гироскоп
    Serial.println("Initializing MPU6050...");
    gyro = new SmoothGyro();

    // Проверяем соединение
    Serial.println(gyro->testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // Обнуляем переменные
    dXPos = 0;
    dYPos = 0;
    dZPos = 0;
    prevXPos = 0;
    prevYPos = 0;
    prevZPos = 0;
    yDirection = 0;
}

/*
   Цикл обработки событий
*/
void loop() {
    // Обрабатываем сдвиги
    int16_t xPos = gyro->getXPosition();
    int16_t yPos = gyro->getZPosition();
    int16_t zPos = gyro->getYPosition();

    dXPos = xPos - prevXPos;
    dYPos = yPos - prevYPos;
    dZPos = zPos - prevZPos;

    prevXPos = xPos;
    prevYPos = yPos;
    prevZPos = zPos;

    yDirection = getDirection(yDirection, dYPos);

    // Переменная - текущие положения датчиков, ответ перчатки (статус), разделителем будет служить ":"
    String status = "";
    // Добавляем к статусу состояние каждого пальца
//    status += (digitalRead(FINGER_1_PIN) == 1) ? "A," : "N,";
//    status += (digitalRead(FINGER_2_PIN) == 1) ? "A," : "N,";
//    status += (digitalRead(FINGER_3_PIN) == 1) ? "A," : "N,";
//    status += (digitalRead(FINGER_4_PIN) == 1) ? "A," : "N,";

    // Обновляем позицию гироскопа (считываем данные)
    gyro->updatePosition();
    // Записываем поворот вокруг оси X
    status += gyro->getXRotation();
    status += ",";
    // Записываем поворот вокруг оси Y
    status += gyro->getYRotation();
    status += ",";
    // Записываем сдвиг по оси Y
    status += yDirection;
    status += "\n";

    // Передаем результат по серийному порту.
    Serial.print(status);
}

int getDirection(int prevDirection, int16_t delta) {
    if (prevDirection == 0 and delta > 5) {
        return -1;
    } else if (prevDirection == 0 and delta < -4) {
        return 1;
    } else if (delta < 1 and delta > -1) {
        return 0;
    }
}
