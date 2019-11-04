#include <Servo.h>

#define CLAW_SERVO_OPEN 90
#define CLAW_SERVO_CLOSE 180

Servo clawServo;
Servo upDownServo;
Servo xRotationServo;
Servo yRotationServo;

// Режим клешни
// true -> однокнопочный режим; false -> двухкнопочный режим
bool clawMode;

// Предыдущее значение кнопки, переключающей режим.
// Нужно, чтобы при зажатой кнопке не переключать режим каждый такт.
int prevModeValue;

// Текущее состояние клешни
int clawState;

enum { F_INDEX, F_MIDDLE, F_RING, F_PINKY, ROT_X, ROT_Y } dataPositions;

void setup() {
    // Установка управляющих пинов для серв
    clawServo.attach(8);
    upDownServo.attach(9);
    xRotationServo.attach(10);
    yRotationServo.attach(11);

    // Установка серв в их начальные позиции
    clawServo.write(CLAW_SERVO_OPEN);
    clawState = CLAW_SERVO_OPEN;
    clawMode = true;

    Serial.begin(9600);
    bool connectionSuccess = false;
    char lastByte = ' ';

    // Ждём сообщения "ОК\n" согласно нашему протоколу
    while (!connectionSuccess) {
        if (Serial.available()) {
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
        int data[7];
        parseData(input, data);

        if (data[F_PINKY] == 1 and prevModeValue != 1) {
            clawMode = !clawMode; // Переключаем режим на обратный
        }
        prevModeValue = data[F_PINKY];

        if (clawMode) {
            // Однокнопочный режим управления клешней
            // Читаем данные о замыкании первого пальца и поворачиваем серво
            if (data[F_INDEX] == 1) {
                // Если клешня открыта - закрываем, если нет - ничего не делаем
                if (clawState == CLAW_SERVO_OPEN) {
                    clawServo.write(CLAW_SERVO_CLOSE);
                    clawState = CLAW_SERVO_CLOSE;
                }
            } else {
                if (clawState == CLAW_SERVO_CLOSE) {
                    clawServo.write(CLAW_SERVO_OPEN);
                    clawState = CLAW_SERVO_OPEN;
                }
            }
        } else {
            // Двухкнопочный режим управления клешней
            if (data[F_MIDDLE] == 1) {
                if (clawState > CLAW_SERVO_OPEN) {
                    clawState--;
                    clawServo.write(clawState);
                }
            } else if (data[F_RING] == 1) {
                if (clawState < CLAW_SERVO_CLOSE) {
                    clawState++;
                    clawServo.write(clawState);
                }
            }
        }

        // Читаем данные о повороте вокруг оси X в диапазоне (-100, 100) и переводим их в диапазон (0, 180)
        int value = -data[ROT_X];
        int adaptedValue = map(value, -100, 100, 0, 180);
        // Поворачиваем серво
        xRotationServo.write(adaptedValue);

        // Читаем данные о повороте вокруг оси Y в диапазоне (-100, 100) и переводим их в диапазон (0, 180)
        value = -data[ROT_Y];
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
