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
}


void loop() {
    // Ждем, пока сообщение дойдет целиком (8 байт)
    if (Serial.available() >= 8) {
        // Читаем и обрабатываем входные данные
        String input = Serial.readStringUntil('\n');
        Serial.print("I received: ");
        Serial.println(input);
    }
}
