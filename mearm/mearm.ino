#include <Servo.h>

#define CLAW_SERVO_OPEN 90
#define CLAW_SERVO_CLOSE 180
#define UPDOWN_SERVO_UP 170
#define UPDOWN_SERVO_DOWN 10

void parseData(String inputData, int outputData[]);

Servo clawServo;
Servo upDownServo;
Servo xRotationServo;
Servo yRotationServo;

bool clawOpened;
bool upDown; // true -> вверх(up); false -> вниз(down) 

void setup() {
    clawServo.attach(8);
    upDownServo.attach(9);
    xRotationServo.attach(10);
    yRotationServo.attach(11);

    // Установка серв в их начальные позиции
    clawServo.write(CLAW_SERVO_OPEN);
    upDownServo.write(UPDOWN_SERVO_UP);
    clawOpened = true;
    upDown = true;

    Serial.begin(9600);
    
    // Ждем, инициализации устройства управления
    delay(10000);
    Serial.write("ready");
}

void loop() {
    if (Serial.available()) {
        // Формат сообщения: "int,int,int,int,int,int,int\n"
        String input = Serial.readStringUntil('\n');
        int out[7];
        parseData(input, out);

        // Читаем данные о замыкании первого пальца и поворачиваем серво
        if (out[0] == 1) {
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
        int value = -out[4];
        int adaptedValue = (constrain(value, -100, 100) + 100) * 180 / 200;
        // Поворачиваем серво
        xRotationServo.write(adaptedValue);

        // Читаем данные о повороте вокруг оси Y в диапазоне (-100, 100) и переводим их в диапазон (0, 180)
        value = -out[5];
        adaptedValue = (constrain(value, -100, 100) + 100) * 180 / 200;
        // Поворачиваем серво
        yRotationServo.write(adaptedValue);

        // Читаем данные о движении вверх/вниз и поворачиваем серво
        if (out[6] > 0) {
            if (!upDown) {
                upDownServo.write(UPDOWN_SERVO_UP);
                upDown = true;
            }
        } else if(out[6] < 0) {
            if (upDown) {
                upDownServo.write(UPDOWN_SERVO_DOWN);
                upDown = false;
            }
        }
    }
}

/*
 * Функция для чтения данных из строки типа "int,int,int,int,int,int,int"
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
