#include <SPI.h>
#include <RF24.h>

RF24 radio(8, 9); // Pines CE y CSN del nRF24L01

// Dirección del canal de comunicación (5 bytes)
const byte address[6] = "OCHO8";

// Estructura para almacenar los datos recibidos del transmisor
struct SensorData {
  float temperature; // Temperatura
  float pressure;    // Presión atmosférica
  float altitude;    // Altitud
  float humidity;     // Humedad
  float latitude;    // Latitud
  float longitude;   // Longitud
};

SensorData currentData;
unsigned long lastReceivedTime = 0;
const unsigned long timeoutDuration = 2000;

// Configuración del buzzer activo
const int buzzerPin = 7;
bool alarmaActiva = false;
unsigned long lastBuzzerToggle = 0;
bool buzzerState = false;

void setup() {
    Serial.begin(9600);

    // Inicializar radio nRF24L01
    if (!radio.begin()) {
        Serial.println("Fallo de inicialización de la radio");
        while (1);
    }

    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_250KBPS);
    radio.setChannel(4);
    radio.openReadingPipe(1, address);
    radio.startListening();

    // Inicializar variables
    currentData.temperature = 0;
    currentData.pressure = 0;
    currentData.altitude = 0;
    currentData.humidity = 0;
    currentData.latitude = 0;
    currentData.longitude = 0;

    // Configurar buzzer
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, LOW);
}

void loop() {
    // Verificar si hay datos disponibles
    if (radio.available()) {
        SensorData data;

        radio.read(&data, sizeof(SensorData));
        currentData = data;
        lastReceivedTime = millis();
    }

    // Si no se reciben datos en el tiempo límite, resetear variables
    if (millis() - lastReceivedTime > timeoutDuration) {
        currentData.temperature = 0;
        currentData.pressure = 0;
        currentData.altitude = 0;
        currentData.humidity = 0;
        currentData.latitude = 0;
        currentData.longitude = 0;
    }

    // Enviar datos por serial en formato CSV
    Serial.print(currentData.temperature); Serial.print(",");
    Serial.print(currentData.pressure); Serial.print(",");
    Serial.print(currentData.altitude); Serial.print(",");
    Serial.print(currentData.humidity); Serial.print(",");
    Serial.print(currentData.latitude, 6); Serial.print(",");
    Serial.println(currentData.longitude, 6);

    // Control del buzzer por temperatura
    if (currentData.temperature >= 30) {
        alarmaActiva = true;
    }

    if (alarmaActiva) {
        if (millis() - lastBuzzerToggle >= 500) {
            lastBuzzerToggle = millis();
            buzzerState = !buzzerState;
            digitalWrite(buzzerPin, buzzerState);
        }
    } else {
        digitalWrite(buzzerPin, LOW);
    }

    delay(500);
}
