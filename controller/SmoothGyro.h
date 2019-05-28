#pragma once
#include <MPU6050.h>
#include <math.h>

// Количество различимых углов наклона на выходе
#define POSITIONS 100
// Количество различимых углов наклона гироскопа
#define DEFAULT_POSITIONS 2048
// Погрешность измерений гироскопа (в исходных единицах)
#define DELTA_ANGLE 100
// Количество сглаживаний измерений
#define SMOOTH_TIMES 10

class SmoothGyro {
    public:
        SmoothGyro(MPU6050* mpu);
        ~SmoothGyro();

        // Обновить значения
        void updatePosition();

        // Получить значения
        int16_t getX();
        int16_t getY();
        int16_t getZ();

    private:
        // Сумма всех последних значений
        int32_t memX, memY, memZ;
        // Текущее количество последних значений
        int16_t updX, updY, updZ;

        // Гироскоп
        MPU6050* mpu;

        // Попробовать обновить значение
        void updateVar(int16_t newVal, int32_t* mem, int16_t* upd);
};
