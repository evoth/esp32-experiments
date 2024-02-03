#include <Adafruit_BME280.h>
#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

char *dataFilePath = "/data.csv";
bool isRecording = false;
bool isTouching = false;

void writeFile(const char *path, const char *message) {
    Serial.printf("Writing file: %s\n", path);

    File file = SD.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    if (file.print(message)) {
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(const char *path, const char *message) {
    Serial.printf("Appending to file: %s\n", path);

    File file = SD.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for appending");
        return;
    }
    if (file.print(message)) {
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void printValues() {
    char message[32];
    snprintf(message, sizeof(message), "%lu,%.2f,%.2f,%.2f\n", millis(), bme.readTemperature(), bme.readPressure() / 100.0F, bme.readHumidity());
    Serial.println(message);
    appendFile(dataFilePath, message);
}

void setup() {
    Serial.begin(115200);
    delay(500);
    while (!Serial) {
        ;
    }  // wait for serial port to connect. Needed for native USB port only
    Serial.println("Initializing SD card...");
    if (!SD.begin()) {
        Serial.println("initialization failed!");
        return;
    }
    Serial.println("initialization done.");

    writeFile(dataFilePath, "Time (ms),Temperature (C),Pressure (hPa),Humidity (%)\n");

    if (!bme.begin(0x76)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1)
            ;
    }
}

void loop() {
    if (touchRead(4) < 75) {
        if (!isTouching) {
            isTouching = true;
            isRecording = !isRecording;
            Serial.printf("Touch sensed! Recording %s.\n", isRecording ? "started" : "stopped");
        }
    } else {
        isTouching = false;
    }
    if (isRecording) {
        printValues();
    }
    delay(1000);
}