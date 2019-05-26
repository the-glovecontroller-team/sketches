#include "SmoothGyro.h"

SmoothGyro::SmoothGyro(MPU6050* mpu) {
    this->mpu = mpu; 
}

SmoothGyro::~SmoothGyro() {
    delete mpu;
}

void SmoothGyro::updatePosition() {
    // Считываем значения
    int16_t rX, rY, rZ;
    int16_t gx_raw, gy_raw, gz_raw;
    mpu->getMotion6(&rX, &rY, &rZ, &gx_raw, &gy_raw, &gz_raw);

    // Обновляем значения
    updateVar(rX, &memX, &updX);
    updateVar(rY, &memY, &updY);
    updateVar(rZ, &memZ, &updZ);
}

void SmoothGyro::updateVar(int16_t newVal, int32_t* mem, int16_t* upd) {
    // Если новое значение больше усредненного текущего на порог допустимого отклонения, то обновляем текущее.
    if (abs(newVal - round(*mem / *upd)) > DELTA_ANGLE) {
        *upd = 1;
        *mem = newVal;
    } 
    // Если новое значение не отличается от текущего значительно, а также если текущее значение не сглаживалось SMOOTH_TIMES, то сглаживаем предыдущее
    else if (*upd < SMOOTH_TIMES) {
        *upd++;
        *mem += newVal;
    }
}

// Возвращаем красивое удобное значение: среднее и переведенное в выбранный диапазон
int16_t SmoothGyro::getX() {
    return memX * POSITIONS / (updX * DEFAULT_POSITIONS);
}

int16_t SmoothGyro::getY() {
    return memY * POSITIONS / (updY * DEFAULT_POSITIONS);
}

int16_t SmoothGyro::getZ() {
    return memZ * POSITIONS / (updZ * DEFAULT_POSITIONS);
}