#include <Arduino.h>
#include <Math.h>
#include "SmoothGyro.h"

#define FINGER_1_PIN 8
#define FINGER_2_PIN 9
#define FINGER_3_PIN 10
#define FINGER_4_PIN 11

// Задержка отправки данных, мс
const int T_OUT = 100; 

SmoothGyro* gyro;

long int t_next;

long int last_t;

/*
 * Подготовка устройства
 */
void setup() {
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
  last_t = millis();
}

/*
 * Цикл обработки событий
 */
void loop() {
  long int t = millis();
  
  if (t_next < t) {
    // Переменная - текущие положения датчиков, ответ перчатки (статус), разделителем будет служить ":"
    String status = "";
    // Добавляем к статусу состояние каждого пальца
    // Формат вывода временно изменен для удобного выведения данных на плоттер
//    status += (digitalRead(FINGER_1_PIN) == 1) ? "A:" : "N:";
//    status += (digitalRead(FINGER_2_PIN) == 1) ? "A:" : "N:";
//    status += (digitalRead(FINGER_3_PIN) == 1) ? "A:" : "N:";
//    status += (digitalRead(FINGER_4_PIN) == 1) ? "A:" : "N:";

    // Обновляем позицию гироскопа (считываем данные)
    gyro->updatePosition();
    // Записываем поворот вокруг оси X
    status += gyro->getX();
    status += ",";
    // Записываем поворот вокруг оси Y
    status += gyro->getY();
    status += ",";
    // Записываем поворот вокруг оси Z
    status += gyro->getPosX(t - last_t);
    status += "\n";

    // Передаем результат по серийному порту. 
    // Важно передавать результат единым целым, чтобы получателям было удобно его читать
    Serial.print(status);

    t_next = t + T_OUT;
    last_t = t;
  }
}
