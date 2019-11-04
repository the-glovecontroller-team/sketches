#include "AccelGyroController.h"

#define FINGER_1_PIN 8
#define FINGER_2_PIN 9
#define FINGER_3_PIN 10
#define FINGER_4_PIN 11
#define STATUS_LED_PIN 12
// Модуль максимального значения возвращаемых данных
#define MAX_OUTPUT_VALUE 100
// Модуль максимального значения данных с MPU6050 (определен экспериментально)
#define MAX_INPUT_VALUE 17000
// Вычисляем множитель для перевода значений в нужный диапазон
int divider = MAX_INPUT_VALUE / MAX_OUTPUT_VALUE;

// Ширина окна для сглаживаний измерений
#define MAX_WINDOW_WIDTH 8
int xAccelValues[MAX_WINDOW_WIDTH] = {0};
int yAccelValues[MAX_WINDOW_WIDTH] = {0};
int xAccelUpdates = 0;
int yAccelUpdates = 0;

AccelGyroController* mpu;

/*
   Подготовка устройства
*/
void setup() {
    // Устанавливаем пины кнопок в режим ввода
    pinMode(FINGER_1_PIN, INPUT_PULLUP);
    pinMode(FINGER_2_PIN, INPUT_PULLUP);
    pinMode(FINGER_3_PIN, INPUT_PULLUP);
    pinMode(FINGER_4_PIN, INPUT_PULLUP);
    pinMode(STATUS_LED_PIN, OUTPUT);

    Serial.begin(9600);
    Serial.println("Initializing...");

    mpu = new AccelGyroController();
    // Проверяем подключение к MPU6050
    bool initializedGood = mpu->testConnection();

    if (initializedGood) {
        // Сообщаем подключенному устройству, что мы готовы отправлять данные
        Serial.println("OK");
        digitalWrite(STATUS_LED_PIN, LOW);
    } else {
        // Ошибка! Мы не можем отправлять данные
        Serial.println("ERROR : MPU6050 connection failed.");
        digitalWrite(STATUS_LED_PIN, HIGH);
    }

    while (!Serial.available() or !initializedGood) {
        // Дожидаемся ответа от подключенного устройства, что оно готово принимать данные
    }
}

/*
   Цикл обработки событий
*/
void loop() {

    // message - текущие положения датчиков, ответ перчатки, разделителем будет служить ","
    String message = "";
    // Добавляем к сообщению состояние каждого пальца
    // Если на пине "0" -> палец замкнут -> в сообщение записываем "1", иначе - "0"
    message += (digitalRead(FINGER_1_PIN) == 0) ? "1," : "0,";
    message += (digitalRead(FINGER_2_PIN) == 0) ? "1," : "0,";
    message += (digitalRead(FINGER_3_PIN) == 0) ? "1," : "0,";
    message += (digitalRead(FINGER_4_PIN) == 0) ? "1," : "0,";

    // Обрабатываем данные с акселерометра
    int accelX, accelY, accelZ;
    mpu->getAcceleration(&accelX, &accelY, &accelZ);

    updateValue(accelX, xAccelValues, &xAccelUpdates);
    updateValue(accelY, yAccelValues, &yAccelUpdates);

    // Добавляем к сообщению сглаженное значение поворота вдоль оси Х
    long int sum = 0;
    for (int i = 0; i < xAccelUpdates; i++) {
        sum += xAccelValues[i];
    }
    message += constrain((int)(sum / (xAccelUpdates * divider)), -MAX_OUTPUT_VALUE, MAX_OUTPUT_VALUE);

    message += ",";

    // Добавляем к сообщению сглаженное значение поворота вдоль оси Y
    sum = 0;
    for (int i = 0; i < yAccelUpdates; i++) {
        sum += yAccelValues[i];
    }
    message += constrain((int)(sum / (yAccelUpdates * divider)), -MAX_OUTPUT_VALUE, MAX_OUTPUT_VALUE);

    // В итоге получаем сообщение вида "a,b,c,d,e,f\n",
    // где a, b, c и d - положения 4х пальцев (0 или 1)
    // e и f - повороты вдоль оси X и Y
    // Передаем результат по последовательному порту.
    Serial.println(message);

}

/*
 * Вспомогательная функция для метода скользящего среднего.
 * valuesWindow[] - окно значений
 * newValue - новое значение для обработки
 * windowWidth - текущая ширина окна
 */
void updateValue(int newValue, int valuesWindow[], int* windowWidth) {
    if (*windowWidth < MAX_WINDOW_WIDTH) {
        (*windowWidth)++; 
    }
    
    for (int i = 0; i < MAX_WINDOW_WIDTH - 1; i++) {
        valuesWindow[i] = valuesWindow[i + 1];
    }
    valuesWindow[MAX_WINDOW_WIDTH - 1] = newValue;

}
