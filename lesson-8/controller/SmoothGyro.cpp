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
    updateVar(accelX, xRotationValues, &xRotationUpdates);
    updateVar(accelY, yRotationValues, &yRotationUpdates);

    // Считываем значения из DMP
    mpu->waitForDmpDataReady();

    // Считаем разницу в ускорении по оси z и запоминаем её
    int16_t zPos = getZAcceleration();
    int16_t dZPos = zPos - prevZPos;
    prevZPos = zPos;
    updateVar(dZPos, zAccelValues, &zAccelUpdates);
}

void SmoothGyro::updateVar(int16_t newValue, int16_t* valuesArray, int* updTimes) {
    // Если массив заполнен, выкидываем первое значение, и записываем новое в конец.
    if (*updTimes == SMOOTH_TIMES) {
        for (int i = 0; i < SMOOTH_TIMES - 1; i++) {
            valuesArray[i] = valuesArray[i + 1];
        }
        valuesArray[SMOOTH_TIMES - 1] = newValue;
    }
    // Иначе просто записываем значение в конец массива
    else {
        valuesArray[*updTimes] = newValue;
        (*updTimes)++;
    }
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
