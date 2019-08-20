#include <Arduino.h>
#include "SmoothGyro.h"

#define FINGER_1_PIN 8
#define FINGER_2_PIN 9
#define FINGER_3_PIN 10
#define FINGER_4_PIN 11

int getDirection(int prevDirection, int16_t delta);

SmoothGyro* gyro;

int zDirection;

bool enableData;

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
    Serial.println(gyro->testConnection() ? "MPU6050 connection successful. Send a character to start receiving data" : "MPU6050 connection failed");

    // Обнуляем переменные
    zDirection = 0;
    enableData = false;
}

/*
   Цикл обработки событий
*/
void loop() {
    if (enableData == false) {
        if (Serial.available() > 0) {
            enableData = true;
        }
        return;
    }

    // status - текущие положения датчиков, ответ перчатки, разделителем будет служить ","
    String status = "";
    // Добавляем к статусу состояние каждого пальца
    status += (digitalRead(FINGER_1_PIN) == 1) ? "1," : "0,";
    status += (digitalRead(FINGER_2_PIN) == 1) ? "1," : "0,";
    status += (digitalRead(FINGER_3_PIN) == 1) ? "1," : "0,";
    status += (digitalRead(FINGER_4_PIN) == 1) ? "1," : "0,";

    // Обновляем позицию гироскопа (считываем данные)
    gyro->updatePosition();

    zDirection = getDirection(zDirection, gyro->getZAccelerationDelta());

    // Записываем поворот вокруг оси X
    status += gyro->getXRotation();
    status += ",";
    // Записываем поворот вокруг оси Y
    status += gyro->getYRotation();
    status += ",";
    // Записываем сдвиг по оси Z
    status += zDirection;

    // В итоге получаем сообщение вида "a,b,c,d,e,f,g\n",
    // где a, b, c и d - положения 4х пальцев (0 или 1)
    // e, f - наклоны вдоль осей X и Y (от -100 до 100)
    // g - сдвиг по оси z (-1 - вниз, 0 - нет сдвига, 1 - вверх)
    // Передаем результат по последовательному порту.
    Serial.println(status);
}

int getDirection(int prevDirection, int16_t delta) {
    if (prevDirection == 0 and delta > 9) {
        return -1;
    } else if (prevDirection == 0 and delta < -8) {
        return 1;
    } else if (delta < 1 and delta > -1) {
        return 0;
    }
}
