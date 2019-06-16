#include <Arduino.h>
#include <MPU6050.h>
#include <Math.h>

// Подключаем библиотеку Arduino Wire, если в I2Cdev.h используется реализация I2Cdev I2CDEV_ARDUINO_WIRE
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  #include "Wire.h"
#endif

#include "SmoothGyro.h"

#define FINGER_1_PIN 8
#define FINGER_2_PIN 9
#define FINGER_3_PIN 10
#define FINGER_4_PIN 11

// Задержка отправки данных, мс
const int T_OUT = 100; 

SmoothGyro* gyro;

long int t_next;


/*
 * Подготовка устройства
 */
void setup() {
  // Подключаемся к I2C шине (библиотека I2Cdev не делает это автоматически)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif

  pinMode(FINGER_1_PIN, INPUT);
  pinMode(FINGER_2_PIN, INPUT);
  pinMode(FINGER_3_PIN, INPUT);
  pinMode(FINGER_4_PIN, INPUT);
  

  Serial.begin(9600);
  
  // Инициализируем гироскоп
  Serial.println("Initializing I2C devices...");

  // Подключаемся к гироскопу, инициализируем его
  MPU6050* mpu = new MPU6050();
  mpu->initialize();
  
  // Проверяем соединение
  Serial.println(mpu->testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  // Создаем удобную обертку над гироскопом, сглаживающую показания
  gyro = new SmoothGyro(mpu);
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
    status += (digitalRead(FINGER_1_PIN) == 1) ? "A:" : "N:";
    status += (digitalRead(FINGER_2_PIN) == 1) ? "A:" : "N:";
    status += (digitalRead(FINGER_3_PIN) == 1) ? "A:" : "N:";
    status += (digitalRead(FINGER_4_PIN) == 1) ? "A:" : "N:";

    // Обновляем позицию гироскопа (считываем данные)
    gyro->updatePosition();
    // Записываем поворот вокруг оси X
    status += gyro->getX();
    status += ":";
    // Записываем поворот вокруг оси Y
    status += gyro->getY();
    status += ":";
    // Записываем поворот вокруг оси Z
    status += gyro->getZ();
    status += ":\n:";

    // Передаем результат по серийному порту. 
    // Важно передавать результат единым целым, чтобы получателям было удобно его читать
    Serial.print(status);

    t_next = t + T_OUT;
  }
}
