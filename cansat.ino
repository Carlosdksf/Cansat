#include <SPI.h>
#include <RF24.h>
#include <Adafruit_BME280.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <PWMServo.h>

#define SEALEVELPRESSURE_HPA (1013.25)

// Pines
#define GPS_RX 4
#define GPS_TX 3
const int buzzerPin = 5;

// Módulos
RF24 radio(8, 9);
Adafruit_BME280 bme;

SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
TinyGPSPlus gps;
PWMServo myServo;

// RF24
const byte address[6] = "OCHO8";

// Coordenadas
struct Coordinates {
    float latitude;
    float longitude;
};

// Datos del sensor
struct SensorData {
    float temperature;
    float pressure;
    float altitude;
    float humidity;
    Coordinates coords;
};

// Variables del buzzer
bool alarmaActiva = false;
unsigned long lastBuzzerToggle = 0;
bool buzzerState = false;

// Variables de GPS
float lastValidLat = 0.0;
float lastValidLon = 0.0;
unsigned long lastGpsCheck = 0;
const unsigned long gpsInterval = 1000; // 1 segundo

void setup() {
    Serial.begin(9600);
    gpsSerial.begin(9600);

    if (!bme.begin()) {
        Serial.println("Error al inicializar el BME280");
        while (1);
    }

    if (!radio.begin()) {
        Serial.println("Error al inicializar el nRF24L01");
        while (1);
    }

    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_250KBPS);
    radio.setChannel(4);
    radio.openWritingPipe(address);
    radio.stopListening();

    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, LOW);

    // Configuración del servo
    myServo.attach(10); //Asociar el servo al pin 10
    myServo.write(0); // Posición inicial en 0 grados
}

void loop() {
    SensorData data;

    // Leer sensores BME280

    data.temperature = bme.readTemperature();
    data.pressure = bme.readPressure() / 100.0F;
    data.altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    data.humidity = bme.readHumidity();

    // Lectura de datos del GPS cada 1 segundo
    unsigned long currentMillis = millis();
    if (currentMillis - lastGpsCheck >= gpsInterval) {
        lastGpsCheck = currentMillis;

        // Procesar datos entrantes del GPS durante 1 segundo
        unsigned long gpsReadStart = millis();
        while (millis() - gpsReadStart < 1000) {
            while (gpsSerial.available()) {
                gps.encode(gpsSerial.read());
            }
        }

        // Validar y guardar coordenadas
        if (gps.location.isValid() && gps.location.isUpdated()) {
            lastValidLat = gps.location.lat();
            lastValidLon = gps.location.lng();
            Serial.println(" GPS actualizado correctamente.");
        } else {
            Serial.println("GPS no actualizado, se mantienen últimas coordenadas válidas.");
        }
    }

    // Usar las últimas coordenadas válidas
    data.coords.latitude = lastValidLat;
    data.coords.longitude = lastValidLon;

    // Enviar datos por nRF24L01
    if (radio.write(&data, sizeof(SensorData))) {
        Serial.println(" Datos enviados con éxito");
    } else {
        Serial.println(" Fallo en el envío de datos");
    }

    // Mostrar datos por serial
    Serial.print("Temp: "); Serial.print(data.temperature); Serial.print(" °C | ");
    Serial.print("Pres: "); Serial.print(data.pressure); Serial.print(" hPa | ");
    Serial.print("Alt: "); Serial.print(data.altitude); Serial.print(" m | ");
    Serial.print("Hum: "); Serial.print(data.humidity); Serial.print(" % | ");
    Serial.print("Lat: "); Serial.print(data.coords.latitude, 6); Serial.print(" | ");
    Serial.print("Lon: "); Serial.println(data.coords.longitude, 6);

    // Alarma por altitud
    if (data.altitude >= 1700) {
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

    // Control del servo según la altitud
    if (data.altitude >= 1700.0) {
        myServo.write(180); // Mover a 180 grados
    }
}
