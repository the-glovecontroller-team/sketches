#include "SmoothGyro.h"

SmoothGyro::SmoothGyro() {
    // Подключаемся к гироскопу, инициализируем его
    mpu = new AccelGyroController(true);

    //Инициализируем переменные
    xRotationUpdates = 0;
    yRotationUpdates = 0;
    // Вычисляем множитель для перевода значений в нужный диапазон
    divider = DEFAULT_POSITIONS / POSITIONS;
}

SmoothGyro::~SmoothGyro() {
    delete mpu;
}

bool SmoothGyro::testConnection() {
    return mpu->testConnection();
}

void SmoothGyro::updatePosition() {
    // Считываем значения с датчиков напрямую
    int16_t accelX, accelY, accelZ;
    int16_t gyroX, gyroY, gyroZ;
    mpu->getMotion6(&accelX, &accelY, &accelZ, &gyroX, &gyroY, &gyroZ);

    // Запоминаем значения с датчиков для сглаживания
    updateVar(accelX, arrX, &xRotationUpdates);
    updateVar(accelY, arrY, &yRotationUpdates);

    // Считываем значения из DMP
    mpu->waitForDmpDataReady();
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
int16_t SmoothGyro::getXRotation() {
    int32_t sum = 0;
    for (int i = 0; i < xRotationUpdates; i++) {
        sum += arrX[i];
    }
    return (int16_t)(sum / (xRotationUpdates * divider));
}

int16_t SmoothGyro::getYRotation() {
    int32_t sum = 0;
    for (int i = 0; i < yRotationUpdates; i++) {
        sum += arrY[i];
    }
    return (int16_t)(sum / (yRotationUpdates * divider));
}

int16_t SmoothGyro::getXPosition() {
    return mpu->getLinearAccelerationInWorldX() / 100;
}

int16_t SmoothGyro::getYPosition() {
    return mpu->getLinearAccelerationInWorldY() / 100;
}

int16_t SmoothGyro::getZPosition() {
    return mpu->getLinearAccelerationInWorldZ() / 100;
}
