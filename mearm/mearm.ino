#include <Servo.h>

Servo clawServo;
Servo upDownServo;
Servo xRotationServo;
Servo yRotationServo;

void parseData(String inputData, int outputData[]);


void setup() {
    clawServo.attach(8);
    upDownServo.attach(9);
    xRotationServo.attach(10);
    yRotationServo.attach(11);

    Serial.begin(9600);
    
    // Ждем, инициализации устройства управления
    delay(10000);
    Serial.write("ready");
}

void loop() {
    if (Serial.available()) {
        // Формат сообщения: "число,число,число\n"
        String input = Serial.readStringUntil('\n');
        int out[3];
        parseData(input, out);

        // Читаем данные о повороте вокруг оси X в диапазоне (-100, 100) и переводим их в диапазон (0, 180)
        int value = -out[0];
        int adaptedValue = (constrain(value, -100, 100) + 100) * 180 / 200;
        // Поворачиваем серво
        xRotationServo.write(value);

        // Читаем данные о повороте вокруг оси Y в диапазоне (-100, 100) и переводим их в диапазон (0, 180)
        value = -out[1];
        adaptedValue = (constrain(value, -100, 100) + 100) * 180 / 200;
        // Поворачиваем серво
        yRotationServo.write(value);

        // Читаем данные о движении вверх/вниз и поворачиваем серво
        if (out[2] > 0) {
            upDownServo.write(170);
        } else if(out[2] < 0) {
            upDownServo.write(10);
        }
    }
}

/*
 * Функция для чтения данных из строки типа "число,число,число"
 * inputData - входная строка
 * outputData[] - целочисленный массив, в который сохраняются полученные значения
 */
void parseData(String inputData, int outputData[]) {
    int strIndex[] = { 0, -1 };
    int maxIndex = inputData.length() - 1;
    int outIndex = 0;

    for (int i = 0; i <= maxIndex; i++) {
        if (inputData.charAt(i) == ',' || i == maxIndex) {
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
            outputData[outIndex++] = inputData.substring(strIndex[0], strIndex[1]).toInt();
        }
    }
}
