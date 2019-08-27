#include "SmoothGyro.h"

SmoothGyro::SmoothGyro() {
    // Подключаемся к гироскопу, инициализируем его
    mpu = new AccelGyroController(true);

    //Инициализируем переменные
    xRotationUpdates = 0;
    yRotationUpdates = 0;
    zAccelUpdates = 0;
    
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
    mpu->getAcceleration(&accelX, &accelY, &accelZ);

    // Запоминаем значения с датчиков для сглаживания
    updateValue(accelX, xRotationValues, &xRotationUpdates);
    updateValue(accelY, yRotationValues, &yRotationUpdates);

    // Считываем значения из DMP
    mpu->waitForDmpDataReady();

    // Считаем разницу в ускорении по оси z и запоминаем её
    int16_t zPos = getZAcceleration();
    int16_t dZPos = zPos - prevZPos;
    prevZPos = zPos;
    updateValue(dZPos, zAccelValues, &zAccelUpdates);
}

void SmoothGyro::updateValue(int16_t newValue, int16_t valuesWindow[], int* windowWidth) {
    if (*windowWidth < SMOOTH_TIMES) {
        (*windowWidth)++; 
    }
    
    for (int i = 0; i < SMOOTH_TIMES - 1; i++) {
        valuesWindow[i] = valuesWindow[i + 1];
    }
    valuesWindow[SMOOTH_TIMES - 1] = newValue;
}


// Возвращаем сглаженное значение в заданном диапазоне
int SmoothGyro::getXRotation() {
    int32_t sum = 0;
    for (int i = 0; i < xRotationUpdates; i++) {
        sum += xRotationValues[i];
    }
    return constrain((int)(sum / (xRotationUpdates * divider)), -100, 100);
}

int SmoothGyro::getYRotation() {
    int32_t sum = 0;
    for (int i = 0; i < yRotationUpdates; i++) {
        sum += yRotationValues[i];
    }
    return constrain((int)(sum / (yRotationUpdates * divider)), -100, 100);
}

// Для данных, полученных с DMP, сглаживание не требуется - оно выполняется на самом DMP
int SmoothGyro::getXAcceleration() {
    return mpu->getLinearAccelerationInWorldX() / 100;
}

int SmoothGyro::getYAcceleration() {
    return mpu->getLinearAccelerationInWorldY() / 100;
}

int SmoothGyro::getZAcceleration() {
    return mpu->getLinearAccelerationInWorldZ() / 100;
}

// Сглаживание здесь необязательно, просто чтобы убедиться, что не будет никаких выбросов
int16_t SmoothGyro::getZAccelerationDelta() {
    int32_t sum = 0;
    for (int i = 0; i < zAccelUpdates; i++) {
        sum += zAccelValues[i];
    }
    return (int16_t)(sum / zAccelUpdates);
}
