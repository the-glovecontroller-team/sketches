#ifndef SMOOTH_ACCEL
#define SMOOTH_ACCEL

#include "AccelGyroController.h"

// Модуль максимального значения возвращаемых данных
#define MAX_OUTPUT_VALUE 100
// Модуль максимального значения данных с MPU6050
#define MAX_INPUT_VALUE 17000
// Ширина окна для сглаживаний измерений
#define MAX_WINDOW_WIDTH 5

class SmoothAccel {
    public:
        SmoothAccel();
        ~SmoothAccel();
    
        // Проверить подключение гироскопа
        bool testConnection();
    
        // Обновить значения
        void updateData();
    
        // Получить значения
        int getXAcceleration();
        int getYAcceleration();
    
        int getDmpXAcceleration();
        int getDmpYAcceleration();
        int getDmpZAcceleration();

        int16_t getZAccelerationDelta();
    
    private:
        // Массивы с текущими значениями
        int16_t xAccelValues[MAX_WINDOW_WIDTH] = {0};
        int16_t yAccelValues[MAX_WINDOW_WIDTH] = {0};
        int16_t zAccelValues[MAX_WINDOW_WIDTH] = {0};
        // Текущее количество последних значений
        int xAccelUpdates, yAccelUpdates, zAccelUpdates;
        // Множитель для перевода значений в нужный диапазон
        int divider;
    
        int16_t prevZPos;
    
        // Гироскоп
        AccelGyroController* mpu;
    
        /*
         * Вспомогательная функция для метода скользящего среднего.
         * valuesWindow[] - окно значений
         * newValue - новое значение для обработки
         * windowWidth - текущая ширина окна
         */
        void updateValue(int16_t newValue, int16_t valuesWindow[], int* windowWidth);
};

#endif
