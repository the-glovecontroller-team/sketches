#ifndef SMOOTH_GYRO
#define SMOOTH_GYRO

#include "AccelGyroController.h"

// Количество различимых углов наклона на выходе
#define POSITIONS 100
// Количество различимых углов наклона гироскопа
#define DEFAULT_POSITIONS 17000
// Количество сглаживаний измерений
#define SMOOTH_TIMES 5

class SmoothGyro {
    public:
        SmoothGyro();
        ~SmoothGyro();
    
        // Проверить подключение гироскопа
        bool testConnection();
    
        // Обновить значения
        void updatePosition();
    
        // Получить значения
        int getXRotation();
        int getYRotation();
    
        int getXAcceleration();
        int getYAcceleration();
        int getZAcceleration();

        int16_t getZAccelerationDelta();
    
    private:
        // Массивы с текущими значениями
        int16_t xRotationValues[SMOOTH_TIMES], yRotationValues[SMOOTH_TIMES];
        int16_t zAccelValues[SMOOTH_TIMES];
        // Текущее количество последних значений
        int xRotationUpdates, yRotationUpdates, zAccelUpdates;
        // Множитель для перевода значений в нужный диапазон
        int divider;
    
        int16_t prevZPos;
    
        // Гироскоп
        AccelGyroController* mpu;
    
        // Обновить значение
        void updateVar(int16_t newValue, int16_t* valuesArray, int* updTimes);
};

#endif
