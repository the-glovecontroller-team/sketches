#include "SmoothAccel.h"

#define FINGER_1_PIN 8
#define FINGER_2_PIN 9
#define FINGER_3_PIN 10
#define FINGER_4_PIN 11

SmoothAccel* mpu;

/*
   Подготовка устройства
*/
void setup() {
    // Устанавливаем пины кнопок в режим ввода
    pinMode(FINGER_1_PIN, INPUT_PULLUP);
    pinMode(FINGER_2_PIN, INPUT_PULLUP);
    pinMode(FINGER_3_PIN, INPUT_PULLUP);
    pinMode(FINGER_4_PIN, INPUT_PULLUP);

    Serial.begin(9600);
    Serial.println("Initializing...");

    mpu = new SmoothAccel();
    // Проверяем подключение к MPU6050
    bool initializedGood = mpu->testConnection();

    if (initializedGood) {
        // Сообщаем подключенному устройству, что мы готовы отправлять данные
        Serial.println("OK");
    } else {
        // Ошибка! Мы не можем отправлять данные
        Serial.println("ERROR : MPU6050 connection failed.");
    }

    while (!Serial.available() or !initializedGood) {
        // Дожидаемся ответа от подключенного устройства, что оно готово принимать данные
    }
}

/*
   Цикл обработки событий
*/
void loop() {

    // status - текущие положения датчиков, ответ перчатки, разделителем будет служить ","
    String status = "";
    // Добавляем к статусу состояние каждого пальца
    // Если на пине "0" -> палец замкнут -> в сообщение записываем "1", иначе - "0"
    status += (digitalRead(FINGER_1_PIN) == 0) ? "1," : "0,";
    status += (digitalRead(FINGER_2_PIN) == 0) ? "1," : "0,";
    status += (digitalRead(FINGER_3_PIN) == 0) ? "1," : "0,";
    status += (digitalRead(FINGER_4_PIN) == 0) ? "1," : "0,";

    // Запрашиваем новые данные у акселерометра
    mpu->updateData();
    // Добавляем к статусу данные с акселерометра
    status += mpu->getAccelerationX();
    status += ",";
    status += mpu->getAccelerationY();

    // В итоге получаем сообщение вида "a,b,c,d\n",
    // где a, b, c и d - положения 4х пальцев (0 или 1)
    // Передаем результат по последовательному порту.
    Serial.println(status);

}
