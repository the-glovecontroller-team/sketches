#include "AccelGyroController.h"

#define FINGER_1_PIN 8
#define FINGER_2_PIN 9
#define FINGER_3_PIN 10
#define FINGER_4_PIN 11

AccelGyroController* mpu;

#define SMOOTH_TIMES 5
int xRotationValues[SMOOTH_TIMES];
int yRotationValues[SMOOTH_TIMES];
int xRotationUpdates;
int yRotationUpdates;

/*
   Подготовка устройства
*/
void setup() {
    // Устанавливаем пины кнопок в режим ввода
    pinMode(FINGER_1_PIN, INPUT);
    pinMode(FINGER_2_PIN, INPUT);
    pinMode(FINGER_3_PIN, INPUT);
    pinMode(FINGER_4_PIN, INPUT);

    Serial.begin(9600);
    Serial.println("Initializing...");

    xRotationUpdates = 0;
    yRotationUpdates = 0;

    mpu = new AccelGyroController();
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
    status += (digitalRead(FINGER_1_PIN) == 1) ? "1," : "0,";
    status += (digitalRead(FINGER_2_PIN) == 1) ? "1," : "0,";
    status += (digitalRead(FINGER_3_PIN) == 1) ? "1," : "0,";
    status += (digitalRead(FINGER_4_PIN) == 1) ? "1," : "0,";

    // Обрабатываем данные с акселерометра
    int accelX, accelY, accelZ;
    mpu->getAcceleration(&accelX, &accelY, &accelZ);

    updateVar(accelX, xRotationValues, &xRotationUpdates);
    updateVar(accelY, yRotationValues, &yRotationUpdates);

    // Добавляем к статусу сглаженное значение поворота вдоль оси Х
    long int sum = 0;
    for (int i = 0; i < xRotationUpdates; i++) {
        sum += xRotationValues[i];
    }
    status += (int)(sum / xRotationUpdates);
    
    status += ",";

    // Добавляем к статусу сглаженное значение поворота вдоль оси Y
    sum = 0;
    for (int i = 0; i < yRotationUpdates; i++) {
        sum += yRotationValues[i];
    }
    status += (int)(sum / yRotationUpdates);

    // В итоге получаем сообщение вида "a,b,c,d,e,f\n",
    // где a, b, c и d - положения 4х пальцев (0 или 1)
    // e и f - повороты вдоль оси X и Y
    // Передаем результат по последовательному порту.
    Serial.println(status);

}

void updateVar(int newValue, int* valuesArray, int* updTimes) {
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
