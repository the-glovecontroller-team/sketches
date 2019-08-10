#include "SmoothGyro.h"

SmoothGyro::SmoothGyro() {
    
    // Подключаемся к гироскопу, инициализируем его
    mpu = new AccelGyroController();

    //Инициализируем переменные
    updX = 0; 
    updY = 0;
    updZ = 0;
    divider = DEFAULT_POSITIONS / POSITIONS; // Вычисляем множитель для перевода значений в нужный диапазон

    s = 0;
    a = 0;
    old_a = 0;
    old_s = 0;
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

//    int16_t accelX = mpu->getAccelerationX();

    // Обновляем значения
    updateVar(rX, arrX, &updX);
    updateVar(rY, arrY, &updY);
    updateVar(gz_raw, arrZ, &updZ);
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

double SmoothGyro::getPosX(long int timeElapsed) {
    old_a = a;
    old_s = s;
    a = mpu->getAccelerationX();
    s = (old_a + a) * timeElapsed / 2.0;
    double x = (old_s + s) * timeElapsed / 2.0;
    return x;
}
