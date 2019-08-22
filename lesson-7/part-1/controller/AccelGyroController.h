#ifndef ACCEL_GYRO_CONTROLLER
#define ACCEL_GYRO_CONTROLLER

#include "MPU6050.h"

#define INTERRUPT_PIN 2 // Пин 2 на Arduino Uno и большинстве остальных плат

/**
   This is basically a wrapper on Jeff Rowberg's MPU6050 library.
   The goal is to provide an easy access to the DMP calculated values so kids could use this lib on their own.
   Only simple get-methods are provided. To fully access Jeff's library use getMPU().
   ----
   Это класс-обертка над библиотекой MPU6050 от Jeff Rowberg.
   Класс предназначен для упрощения работы с DMP так, что дети могут работать с этой библиотекой самостоятельно.
   Предоставлены только необходимые get-методы. Для использования полных возможностей библиотеки используйте getMPU().
*/
class AccelGyroController {
    public:
        AccelGyroController();
        AccelGyroController(bool dmpEnabled);
    
        bool testConnection();
        bool isDmpReady();
        void enableDmp();
    
        void getMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz);

        /*
         * Функции этого блока считывают показания акселерметра напрямую
         */
        void getAcceleration(int16_t* x, int16_t* y, int16_t* z);
        int16_t getAccelerationX();
        int16_t getAccelerationY();
        int16_t getAccelerationZ();

        /*
         * Функции этого блока считывают показания гироскопа напрямую
         */    
        void getRotation(int16_t* x, int16_t* y, int16_t* z);
        int16_t getRotationX();
        int16_t getRotationY();
        int16_t getRotationZ();
    
        // Для использования этих функций необходимо включить DMP
    
        // Используйте перед тем, как получить данные с DMP
        void waitForDmpDataReady();

        /*
         * Функции этого блока возвращают значения линейного ускорения, посчитанного на DMP
         */
        void getLinearAcceleration(int16_t* x, int16_t* y, int16_t* z);
        int16_t getLinearAccelerationX();
        int16_t getLinearAccelerationY();
        int16_t getLinearAccelerationZ();
    
        /*
         * Функции этого блока возвращают значения линейного ускорения, посчитанного на DMP
         * В отличие от getLinearAcceleration() переводит значение в "мировые" координаты
         */
        void getLinearAccelerationInWorld(int16_t* x, int16_t* y, int16_t* z);
        int16_t getLinearAccelerationInWorldX();
        int16_t getLinearAccelerationInWorldY();
        int16_t getLinearAccelerationInWorldZ();
    
        // Используйте только если вы знаете, что с этим делать!
        MPU6050* getMPU();
    
    private:
        MPU6050 mpu;
        bool dmpEnabled;
        // return status after each device operation (0 = success, !0 = error)
        uint8_t dmpStatus;
        // holds actual interrupt status byte from MPU
        uint8_t mpuIntStatus;
        // indicates whether MPU interrupt pin has gone high
    
    
        //** Dmp related variables
    
        // expected DMP packet size (default is 42 bytes)
        uint16_t packetSize;
        // count of all bytes currently in FIFO
        uint16_t fifoCount;
        // FIFO storage buffer
        uint8_t fifoBuffer[64];
    
    
        //** Dmp data and calculation results
    
        // [w, x, y, z]         quaternion container
        Quaternion q;
        // [x, y, z]            accel sensor measurements
        VectorInt16 aa;
        // [x, y, z]            gravity-free accel sensor measurements
        VectorInt16 aaReal;
        // [x, y, z]            world-frame accel sensor measurements
        VectorInt16 aaWorld;
        // [x, y, z]            gravity vector
        VectorFloat gravity;
};

static volatile bool mpuInterrupt = false;

static void dmpDataReady() {
    mpuInterrupt = true;
}

#endif
