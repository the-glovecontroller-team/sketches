#ifndef SMOOTH_GYRO
#define SMOOTH_GYRO

#include "AccelGyroController.h"
#include <math.h>

// Количество различимых углов наклона на выходе
#define POSITIONS 100
// Количество различимых углов наклона гироскопа
#define DEFAULT_POSITIONS 17000
// Погрешность измерений гироскопа (в исходных единицах)
#define DELTA_ANGLE 100
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
        int16_t getXRotation();
        int16_t getYRotation();
    
        int16_t getXAcceleration();
        int16_t getYAcceleration();
        int16_t getZAcceleration();

        int16_t getZAccelerationDelta();
    
    private:
        // Массивы с текущими значениями
        int16_t xRotValues[SMOOTH_TIMES], yRotValues[SMOOTH_TIMES], zAccelValues[SMOOTH_TIMES];
        // Текущее количество последних значений
        int16_t xRotationUpdates, yRotationUpdates, zAccelUpdates;
        // Множитель для перевода значений в нужный диапазон
        int16_t divider;
    
        int16_t prevZPos;
    
        // Гироскоп
        AccelGyroController* mpu;
    
        // Обновить значение
        void updateVar(int16_t newVal, int16_t* arr, int16_t* upd);
};

#endif
