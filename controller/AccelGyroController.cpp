#include "I2Cdev.h"

//#include "MPU6050_6Axis_MotionApps20.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

#include "AccelGyroController.h"


AccelGyroController::AccelGyroController() {
    AccelGyroController(false);
}

AccelGyroController::AccelGyroController(bool withDmp) {
    // Подключаемся к I2C шине (библиотека I2Cdev не делает это автоматически)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000); // 400kHz I2C clock. Закомментируйте эту строчку, если возникнут проблемы с компиляцией.
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    mpu = MPU6050(MPU6050_ADDRESS_AD0_LOW);
    mpu.initialize();
    Serial.println(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    dmpEnabled = false;

    if (withDmp) {
        enableDmp();
    }
}

bool AccelGyroController::testConnection() {
    return mpu.testConnection();
}

void AccelGyroController::getMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz) {
    mpu.getMotion6(ax, ay, az, gx, gy, gz);
}

void AccelGyroController::getAcceleration(int16_t* x, int16_t* y, int16_t* z) {
    mpu.getAcceleration(x, y, z);
}

int16_t AccelGyroController::getAccelerationX() {
    return mpu.getAccelerationX();
}

int16_t AccelGyroController::getAccelerationY() {
    return mpu.getAccelerationY();
}

int16_t AccelGyroController::getAccelerationZ() {
    return mpu.getAccelerationZ();
}

void AccelGyroController::getRotation(int16_t* x, int16_t* y, int16_t* z) {
    mpu.getRotation(x, y, z);
}

int16_t AccelGyroController::getRotationX() {
    return mpu.getRotationX();
}

int16_t AccelGyroController::getRotationY() {
    return mpu.getRotationY();
}

int16_t AccelGyroController::getRotationZ() {
    return mpu.getRotationZ();
}

void AccelGyroController::getLinearAcceleration(int16_t* x, int16_t* y, int16_t* z) {
    *x = aaReal.x;
    *y = aaReal.y;
    *z = aaReal.z;
}

int16_t AccelGyroController::getLinearAccelerationX() {
    return aaReal.x;
}

int16_t AccelGyroController::getLinearAccelerationY() {
    return aaReal.y;
}

int16_t AccelGyroController::getLinearAccelerationZ() {
    return aaReal.z;
}

void AccelGyroController::getLinearAccelerationInWorld(int16_t* x, int16_t* y, int16_t* z) {
    *x = aaWorld.x;
    *y = aaWorld.y;
    *z = aaWorld.z;
}

int16_t AccelGyroController::getLinearAccelerationInWorldX() {
    return aaWorld.x;
}

int16_t AccelGyroController::getLinearAccelerationInWorldY() {
    return aaWorld.y;
}

int16_t AccelGyroController::getLinearAccelerationInWorldZ() {
    return aaWorld.z;
}

MPU6050* AccelGyroController::getMPU() {
    return &mpu;
}

void AccelGyroController::enableDmp() {
    if (dmpEnabled) return;

    //TODO Enable DMP here
    dmpStatus = mpu.dmpInitialize();

    // make sure it worked (returns 0 if so)
    if (dmpStatus == 0)
    {
        // Calibration Time: generate offsets and calibrate our MPU6050
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        mpu.PrintActiveOffsets();
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();

        dmpEnabled = true;
    }
    else
    {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        Serial.print(F("DMP Initialization failed with code "));
        Serial.print(dmpStatus);
    }

    
}

void AccelGyroController::waitForDmpDataReady() {
    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize)
    {
        if (mpuInterrupt && fifoCount < packetSize)
        {
            // try to get out of the infinite loop
            fifoCount = mpu.getFIFOCount();
        }
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();
    if (fifoCount < packetSize) {
        //Lets go back and wait for another interrupt. We shouldn't be here, we got an interrupt from another event
        // This is blocking so don't do it   while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    }
    // check for overflow (this should never happen unless our code is too inefficient)
    else if ((mpuIntStatus & _BV(MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) || fifoCount >= 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        //  fifoCount = mpu.getFIFOCount();  // will be zero after reset no need to ask
        Serial.println(F("FIFO overflow!"));

        // otherwise, check for DMP data ready interrupt (this should happen frequently)
    }
    else if (mpuIntStatus & _BV(MPU6050_INTERRUPT_DMP_INT_BIT)) {

        // read a packet from FIFO
        while (fifoCount >= packetSize)
        { // Lets catch up to NOW, someone is using the dreaded delay()!
            mpu.getFIFOBytes(fifoBuffer, packetSize);
            // track FIFO count here in case there is > 1 packet available
            // (this lets us immediately read more without waiting for an interrupt)
            fifoCount -= packetSize;
        }

        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetAccel(&aa, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
        mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);

    }
}
