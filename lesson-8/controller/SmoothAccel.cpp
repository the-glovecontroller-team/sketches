#include "SmoothAccel.h"

SmoothAccel::SmoothAccel() {
    // Подключаемся к гироскопу, инициализируем его
    mpu = new AccelGyroController(true);

    //Инициализируем переменные
    xAccelUpdates = 0;
    yAccelUpdates = 0;
    zAccelUpdates = 0;
    
    // Вычисляем множитель для перевода значений в нужный диапазон
    divider = MAX_INPUT_VALUE / MAX_OUTPUT_VALUE;
}

SmoothAccel::~SmoothAccel() {
    delete mpu;
}

bool SmoothAccel::testConnection() {
    return mpu->testConnection();
}

void SmoothAccel::updateData() {
    // Считываем значения с датчиков напрямую
    int16_t accelX, accelY, accelZ;
    mpu->getAcceleration(&accelX, &accelY, &accelZ);

    // Запоминаем значения с датчиков для сглаживания
    updateValue(accelX, xAccelValues, &xAccelUpdates);
    updateValue(accelY, yAccelValues, &yAccelUpdates);

    // Считываем значения из DMP
    mpu->waitForDmpDataReady();

    // Считаем разницу в ускорении по оси z и запоминаем её
    int16_t zPos = getDmpZAcceleration();
    int16_t dZPos = zPos - prevZPos;
    prevZPos = zPos;
    updateValue(dZPos, zAccelValues, &zAccelUpdates);
}

void SmoothAccel::updateValue(int16_t newValue, int16_t valuesWindow[], int* windowWidth) {
    if (*windowWidth < MAX_WINDOW_WIDTH) {
        (*windowWidth)++; 
    }
    
    for (int i = 0; i < MAX_WINDOW_WIDTH - 1; i++) {
        valuesWindow[i] = valuesWindow[i + 1];
    }
    valuesWindow[MAX_WINDOW_WIDTH - 1] = newValue;
}


// Возвращаем сглаженное значение в заданном диапазоне
int SmoothAccel::getXAcceleration() {
    int32_t sum = 0;
    for (int i = 0; i < xAccelUpdates; i++) {
        sum += xAccelValues[i];
    }
    return constrain((int)(sum / (xAccelUpdates * divider)), -MAX_OUTPUT_VALUE, MAX_OUTPUT_VALUE);
}

int SmoothAccel::getYAcceleration() {
    int32_t sum = 0;
    for (int i = 0; i < yAccelUpdates; i++) {
        sum += yAccelValues[i];
    }
    return constrain((int)(sum / (yAccelUpdates * divider)), -MAX_OUTPUT_VALUE, MAX_OUTPUT_VALUE);
}

// Для данных, полученных с DMP, сглаживание не требуется - оно выполняется на самом DMP
int SmoothAccel::getDmpXAcceleration() {
    return mpu->getLinearAccelerationInWorldX() / 100;
}

int SmoothAccel::getDmpYAcceleration() {
    return mpu->getLinearAccelerationInWorldY() / 100;
}

int SmoothAccel::getDmpZAcceleration() {
    return mpu->getLinearAccelerationInWorldZ() / 100;
}

// Сглаживание здесь необязательно, просто чтобы убедиться, что не будет никаких выбросов
int16_t SmoothAccel::getZAccelerationDelta() {
    int32_t sum = 0;
    for (int i = 0; i < zAccelUpdates; i++) {
        sum += zAccelValues[i];
    }
    return (int16_t)(sum / zAccelUpdates);
}
