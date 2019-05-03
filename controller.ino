#include <MPU6050.h>
#include <I2Cdev.h>
#include <Math.h>

// Подключаем библиотеку Arduino Wire, если в I2Cdev.h используется реализация I2Cdev I2CDEV_ARDUINO_WIRE
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  #include "Wire.h"
#endif

const int DELTA = 5; // Уровень помех
const int T_OUT = 50; // Задержка

long int t_next;

MPU6050 mpu;

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
  
  Serial.begin(9600);
  
  // Инициализируем гироскоп
  Serial.println("Initializing I2C devices...");
  mpu.initialize();
  
  // Проверяем соединение
  Serial.println(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
}

/*
 * Цикл обработки событий
 */
void loop() {
  long int t = millis();
  if (t_next < t) {
    int16_t ax_raw, ay_raw, az_raw, gx_raw, gy_raw, gz_raw;
    float ay;

    if (digitalRead(9) == 1) {
      Serial.print("A:");
    } else {
      Serial.print("N:");
    }
   
    mpu.getMotion6(&ax_raw, &ay_raw, &az_raw, &gx_raw, &gy_raw, &gz_raw);
    
    ay = ay_raw / 2048.0;
    
    if (ay >= DELTA) {
      Serial.print("0:");
    }
    else if (ay <= -DELTA){
      Serial.print("180:");
    } else {
      Serial.print("90:");
    }
    
    ay = ax_raw / 2048.0;
    
    if (ay >= DELTA) {
      Serial.print("0:\n:");
    }
    else if (ay <= -DELTA){
      Serial.print("180:\n:");
    } else {
      Serial.print("90:\n:");
    }

    t_next = t + T_OUT;
  }
}
