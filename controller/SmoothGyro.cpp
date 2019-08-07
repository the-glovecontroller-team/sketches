#include "SmoothGyro.h"
#include <MPU6050.h>

// Подключаем библиотеку Arduino Wire, если в I2Cdev.h используется реализация I2Cdev I2CDEV_ARDUINO_WIRE
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  #include "Wire.h"
#endif

SmoothGyro::SmoothGyro() {
    // Подключаемся к I2C шине (библиотека I2Cdev не делает это автоматически)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
    #endif
    
    // Подключаемся к гироскопу, инициализируем его
    this->mpu = new MPU6050();
    this->mpu->initialize();

    //Инициализируем переменные
    this->updX = 0; 
    this->updY = 0;
    this->updZ = 0;
    this->divider = DEFAULT_POSITIONS / POSITIONS; // Вычисляем множитель для перевода значений в нужный диапазон
}

SmoothGyro::~SmoothGyro() {
    delete mpu;
}

bool SmoothGyro::testConnection() {
  return mpu->testConnection();
}

void SmoothGyro::updatePosition() {
    // Считываем значения
    int16_t rX, rY, rZ;
    int16_t gx_raw, gy_raw, gz_raw;
    mpu->getMotion6(&rX, &rY, &rZ, &gx_raw, &gy_raw, &gz_raw);

    // Обновляем значения
    updateVar(rX, arrX, &updX);
    updateVar(rY, arrY, &updY);
    updateVar(rZ, arrZ, &updZ);
}

void SmoothGyro::updateVar(int16_t newVal, int16_t* arr, int16_t* upd) {
    // Если массив заполнен, выкидываем первое значение, и записываем новое в конец.
    if (*upd == SMOOTH_TIMES) {
        for (int i = 0; i < SMOOTH_TIMES - 1; i++) {
          arr[i] = arr[i + 1];
        }
        arr[SMOOTH_TIMES - 1] = newVal;
    }
    // Иначе просто записываем значение в конец массива
    else {
        arr[*upd] = newVal;
        (*upd)++;
    }
}


// Возвращаем красивое удобное значение: среднее и переведенное в выбранный диапазон
int16_t SmoothGyro::getX() {
    int32_t sum = 0;
    for (int i = 0; i < updX; i++) {
      sum += arrX[i];
    }
    return (int16_t)(sum / (updX * divider));
}

int16_t SmoothGyro::getY() {
    int32_t sum = 0;
    for (int i = 0; i < updY; i++) {
      sum += arrY[i];
    }
    return (int16_t)(sum / (updY * divider));
}

int16_t SmoothGyro::getZ() {
    int32_t sum = 0;
    for (int i = 0; i < updZ; i++) {
      sum += arrZ[i];
    }
    return (int16_t)(sum / (updZ * divider));
}
