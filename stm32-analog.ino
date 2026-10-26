
/*
 * Project: STM32 Temperature and Humidity Monitor
 * Board: STM32F103C8T6 Blue Pill
 * Environment: Wokwi Simulation
 *
 * Features:
 * - DHT22 temperature and humidity monitoring
 * - SSD1306 OLED display
 * - Green and red status LEDs
 * - Buzzer alert
 * - UART serial output
 * - Two-second sampling using millis()
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ---------------- Pin Definitions ----------------
#define GREEN_LED PA1
#define RED_LED   PA2
#define BUZZER_PIN PA3
#define DHT_PIN   PA4

// ---------------- Sensor Configuration ------------
#define DHTTYPE DHT22
#define TEMP_THRESHOLD 30.0f
#define HUMIDITY_THRESHOLD 70.0f

// ---------------- OLED Configuration --------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// ---------------- Objects -------------------------
DHT dht(DHT_PIN, DHTTYPE);

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    OLED_RESET
);

// ---------------- Sampling Configuration ----------
const unsigned long SAMPLE_INTERVAL_MS = 2000;
unsigned long lastSampleTime = 0;

// ---------------- Function Declarations -----------
void displaySplash();
void displayReadings(float temp, float hum,
                     bool tempAlert, bool humAlert);
void displaySensorError();
void readAndProcessSensor();

// ---------------- OLED Splash Screen --------------
void displaySplash()
{
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(10, 10);
    display.println(F("STM32 ENV MONITOR"));

    display.setCursor(18, 26);
    display.println(F("DHT22 & SSD1306"));

    display.setCursor(15, 42);
    display.println(F("Initializing..."));

    display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
    display.display();
}

// ---------------- Display Readings -----------------
void displayReadings(float temp, float hum,
                     bool tempAlert, bool humAlert)
{
    display.clearDisplay();

    // Header
    display.fillRect(0, 0, 128, 12, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.setCursor(8, 2);

    if (tempAlert || humAlert) {
        display.print(F("! ENVIRONMENT ALERT !"));
    } else {
        display.print(F("STM32 ENV MONITOR"));
    }

    // Measurement values
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(4, 18);
    display.print(F("Temp: "));
    display.print(temp, 1);
    display.print(F(" C"));

    if (tempAlert) {
        display.print(F(" [!]"));
    }

    display.setCursor(4, 30);
    display.print(F("Humidity: "));
    display.print(hum, 1);
    display.print(F(" %"));

    if (humAlert) {
        display.print(F(" [!]"));
    }

    display.drawFastHLine(0, 43, 128, SSD1306_WHITE);

    display.setCursor(4, 48);

    if (tempAlert && humAlert) {
        display.print(F("High Temp & Humidity"));
    } else if (tempAlert) {
        display.print(F("ALERT: High Temp!"));
    } else if (humAlert) {
        display.print(F("ALERT: High Humid!"));
    } else {
        display.print(F("Status: NORMAL"));
    }

    display.display();
}

// ---------------- Sensor Error Screen --------------
void displaySensorError()
{
    display.clearDisplay();

    display.fillRect(0, 0, 128, 12, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.setCursor(14, 2);
    display.print(F("SENSOR READ ERROR"));

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(4, 20);
    display.println(F("Failed to read DHT22"));

    display.setCursor(4, 32);
    display.println(F("Check sensor wiring"));

    display.setCursor(4, 48);
    display.println(F("Retrying shortly"));

    display.display();
}

// ---------------- Read and Process Sensor -----------
void readAndProcessSensor()
{
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Check for invalid sensor readings
    if (isnan(temperature) || isnan(humidity)) {
        Serial.println(F("[ERROR] Failed to read DHT22"));

        displaySensorError();

        digitalWrite(GREEN_LED, LOW);
        digitalWrite(RED_LED, HIGH);
        noTone(BUZZER_PIN);
        digitalWrite(BUZZER_PIN, LOW);

        return;
    }

    // Evaluate safety thresholds
    bool tempAlert = (temperature > TEMP_THRESHOLD);
    bool humAlert = (humidity > HUMIDITY_THRESHOLD);

    // Send measurements over UART
    Serial.print(F("[DATA] Temperature: "));
    Serial.print(temperature, 1);
    Serial.print(F(" C | Humidity: "));
    Serial.print(humidity, 1);
    Serial.print(F(" % | Status: "));

    if (tempAlert || humAlert) {
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(RED_LED, HIGH);
        tone(BUZZER_PIN, 1000);

        if (tempAlert && humAlert) {
            Serial.println(F("CRITICAL: High temperature and humidity"));
        } else if (tempAlert) {
            Serial.println(F("WARNING: High temperature"));
        } else {
            Serial.println(F("WARNING: High humidity"));
        }
    } else {
        digitalWrite(GREEN_LED, HIGH);
        digitalWrite(RED_LED, LOW);
        noTone(BUZZER_PIN);
        digitalWrite(BUZZER_PIN, LOW);

        Serial.println(F("NORMAL"));
    }

    displayReadings(temperature, humidity, tempAlert, humAlert);
}

// ---------------- Setup -----------------------------
void setup()
{
    Serial.begin(9600);
    delay(100);

    Serial.println();
    Serial.println(F("================================"));
    Serial.println(F("STM32 Temperature Monitor"));
    Serial.println(F("================================"));

    // Configure GPIO outputs
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    // Initialize I2C and OLED
    Wire.begin();

    if (display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("[OK] OLED initialized"));
        displaySplash();
    } else {
        Serial.println(F("[ERROR] OLED initialization failed"));
    }

    // Initialize DHT22
    dht.begin();
    Serial.println(F("[OK] DHT22 initialized"));

    Serial.println(F("[INFO] Sampling interval: 2000 ms"));

    // Take the first measurement immediately
    lastSampleTime = millis() - SAMPLE_INTERVAL_MS;
}

// ---------------- Main Loop ------------------------
void loop()
{
    unsigned long currentTime = millis();

    if (currentTime - lastSampleTime >= SAMPLE_INTERVAL_MS) {
        lastSampleTime = currentTime;
        readAndProcessSensor();
    }
}
