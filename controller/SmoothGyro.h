#pragma once
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
        int16_t getX();
        int16_t getY();
        int16_t getZ();

        double getPosX(long int timeElapsed);

    private:
        // Массивы с текущими значениями
        int16_t arrX[SMOOTH_TIMES], arrY[SMOOTH_TIMES], arrZ[SMOOTH_TIMES];
        // Текущее количество последних значений
        int16_t updX, updY, updZ;
        // Множитель для перевода значений в нужный диапазон
        int16_t divider;

        double s;
        double a;
        double old_a;
        double old_s;

        // Гироскоп
        AccelGyroController* mpu;

        // Обновить значение
        void updateVar(int16_t newVal, int16_t* arr, int16_t* upd);
};
