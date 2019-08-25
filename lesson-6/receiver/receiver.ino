#define LED1_PIN 4
#define LED2_PIN 5
#define LED3_PIN 6
#define LED4_PIN 7

void setup() {
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);
    pinMode(LED4_PIN, OUTPUT);

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
    // Ждем, пока сообщение дойдет целиком (8 байт)
    if (Serial.available() >= 8) {
        // Читаем и обрабатываем входные данные
        String input = Serial.readStringUntil('\n');
        int data[4] = {0};
        parseData(input, data);

        digitalWrite(LED1_PIN, data[0]);
        digitalWrite(LED2_PIN, data[1]);
        digitalWrite(LED3_PIN, data[2]);
        digitalWrite(LED4_PIN, data[3]);
    }
}

/*
 * Функция для чтения данных из строки типа "int,int,int,int"
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
