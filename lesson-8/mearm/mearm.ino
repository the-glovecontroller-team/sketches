#include <Servo.h>

#define CLAW_SERVO_OPEN 90
#define CLAW_SERVO_CLOSE 180

Servo clawServo;
Servo upDownServo;
Servo xRotationServo;
Servo yRotationServo;

bool clawOpened;

void setup() {
    // Установка управляющих пинов для серв
    clawServo.attach(8);
    upDownServo.attach(9);
    xRotationServo.attach(10);
    yRotationServo.attach(11);

    // Установка серв в их начальные позиции
    clawServo.write(CLAW_SERVO_OPEN);
    clawOpened = true;

    Serial.begin(9600);
    bool connectionSuccess = false;
    char lastByte = ' ';

    // Ждём сообщения "ОК\n" согласно нашему протоколу
    while(!connectionSuccess) {
        if(Serial.available()) {
            char incomingByte = Serial.read();
            if (lastByte == 'O' and incomingByte == 'K') {
                // Пропускаем "\n"
                Serial.read();
                Serial.println("OK!");
                break;
            }
            lastByte = incomingByte;
        }
    }
}

void loop() {
    if (Serial.available()) {
        // Формат сообщения: "int,int,int,int,int,int,int\n"
        String input = Serial.readStringUntil('\n');
        int data[6];
        parseData(input, data);

        // Читаем данные о замыкании первого пальца и поворачиваем серво
        if (data[0] == 1) {
            // Если клешня открыта - закрываем, если нет - ничего не делаем
            if (clawOpened) {
                clawServo.write(CLAW_SERVO_CLOSE);
                clawOpened = false;
            }
        } else {
            if (!clawOpened) {
                clawServo.write(CLAW_SERVO_OPEN);
                clawOpened = true;
            }
        }

        // Читаем данные о повороте вокруг оси X в диапазоне (-100, 100) и переводим их в диапазон (0, 180)
        int value = -data[4];
        int adaptedValue = map(value, -100, 100, 0, 180);
        // Поворачиваем серво
        xRotationServo.write(adaptedValue);

        // Читаем данные о повороте вокруг оси Y в диапазоне (-100, 100) и переводим их в диапазон (0, 180)
        value = -data[5];
        adaptedValue = map(value, -100, 100, 0, 180);
        // Поворачиваем серво
        yRotationServo.write(adaptedValue);
        
        adaptedValue = map(value, -100, 100, 30, 150);
        upDownServo.write(adaptedValue);
    }
}

/*
 * Функция для чтения данных из строки типа "int,int,int,int,int,int,int"
 * inputData - входная строка
 * outputData[] - целочисленный массив, в который сохраняются полученные значения
 */
void parseData(String inputData, int outputData[]) {
    int foundStrIndex[] = { 0, -1 };
    int maxIndex = inputData.length() - 1;
    int outputArrayIndex = 0;

    for (int i = 0; i <= maxIndex; i++) {
        if (inputData.charAt(i) == ',' || i == maxIndex) {
            foundStrIndex[0] = foundStrIndex[1] + 1;
            foundStrIndex[1] = (i == maxIndex) ? i+1 : i;
            outputData[outputArrayIndex++] = inputData.substring(foundStrIndex[0], foundStrIndex[1]).toInt();
        }
    }
}
