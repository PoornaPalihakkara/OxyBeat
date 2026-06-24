#define BLYNK_TEMPLATE_ID   "your_template_id"
#define BLYNK_TEMPLATE_NAME "your_template_name"

char auth[] = "your_auth_token";
char ssid[] = "your_wifi_name";
char pass[] = "your_wifi_password";

#include <U8g2lib.h>
#include <Wire.h>
#include "MAX30105.h" // SparkFun library
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Pin Definitions
// DS18B20 and DHT11 sensors
#define ONE_WIRE_BUS 25        // DS18B20 data pin
#define DHTPIN 26              // DHT11 data pin

// Buzzers and LEDs
#define HUMAN_BODY_BUZZER 14   // Buzzer for body temperature
#define ROOM_TEMP_GOOD_LED 27  // Green LED for good room temperature
#define GREEN_LED 12           // Green LED for good humidity
#define RED_LED 13             // Red LED for close to monitor
#define POWER_LED 5            // Power LED for MAX30102

// Ultrasonic sensor
#define ULTRASONIC_TRIG_PIN 18 // Ultrasonic Trigger pin
#define ULTRASONIC_ECHO_PIN 19 // Ultrasonic Echo pin

// Sensor Thresholds & Constants
#define DHTTYPE DHT11          // DHT sensor type
#define ABNORMAL_TEMP_MIN 36.1 // Min threshold for abnormal human body temp (°C)
#define ABNORMAL_TEMP_MAX 37.2 // Max threshold for abnormal human body temp (°C)
#define ROOM_TEMP_ALARM_MIN 20.0 // Min threshold for room temp alarm (°C)
#define ROOM_TEMP_ALARM_MAX 22.0 // Max threshold for room temp alarm (°C)
#define GOOD_HUMIDITY_MIN 30.0 // Min threshold for good humidity (%)
#define GOOD_HUMIDITY_MAX 50.0 // Max threshold for good humidity (%)
#define CLOSE_DISTANCE_CM 50   // Distance threshold for red LED (cm)

// Global Variables
long duration;
int distance;
float humanTemp, roomTemp, humidity;
float bpm = 0.0;
float spo2 = 0.0;
byte fingerStatus = 0; // 0 = No finger, 1 = Finger detected

// MAX30102 variables
MAX30105 particleSensor;

// Pulse Oximetry Algorithm Variables
const byte FINGER_ON_SENSOR = 1;
const byte FINGER_OFF_SENSOR = 0;
const int IR_AC_OFFSET = 250000; // IR value when no finger is present

// This is a simple pulse oximetry algorithm.
int i = 0;
float irArray[100]; // Store IR samples
float redArray[100]; // Store Red samples
float spo2_ratio = 0.0;

// Object Initialization
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DHT dht(DHTPIN, DHTTYPE);

// Functions
void readOtherSensors() {
  sensors.requestTemperatures();
  humanTemp = sensors.getTempCByIndex(0);
  roomTemp = dht.readTemperature();
  humidity = dht.readHumidity();

  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Body Temp: ");
  Serial.print(humanTemp);
  Serial.println(" C");
  Serial.print("Room Temp: ");
  Serial.print(roomTemp);
  Serial.print(" C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("BPM: ");
  Serial.print(bpm);
  Serial.print(" | SpO2: ");
  Serial.print(spo2);
  Serial.println(" %");
}

void updateOLED() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvR08_tr);

  u8g2.setCursor(0, 12);
  u8g2.print("Body: ");
  u8g2.print(humanTemp);
  u8g2.print(" C");

  u8g2.setCursor(0, 28);
  u8g2.print("Room: ");
  u8g2.print(roomTemp);
  u8g2.print(" C");
  
  u8g2.setCursor(0, 44);
  u8g2.print("Humid: ");
  u8g2.print(humidity);
  u8g2.print(" %");

  u8g2.setCursor(0, 60);
  if (fingerStatus == 1) {
    u8g2.print("BPM: ");
    u8g2.print(bpm);
    u8g2.print(" | SpO2: ");
    u8g2.print(spo2);
    u8g2.print("%");
  } else {
    u8g2.print("Place finger");
  }
  
  u8g2.sendBuffer();
}

void controlIndicators() {
  if (humanTemp < ABNORMAL_TEMP_MIN || humanTemp > ABNORMAL_TEMP_MAX) {
    tone(HUMAN_BODY_BUZZER, 440);
  } else {
    noTone(HUMAN_BODY_BUZZER);
  }

  if (humidity >= GOOD_HUMIDITY_MIN && humidity <= GOOD_HUMIDITY_MAX) {
    digitalWrite(GREEN_LED, HIGH);
  } else {
    digitalWrite(GREEN_LED, LOW);
  }

  if (roomTemp >= ROOM_TEMP_ALARM_MIN && roomTemp <= ROOM_TEMP_ALARM_MAX) {
    digitalWrite(ROOM_TEMP_GOOD_LED, HIGH);
  } else {
    digitalWrite(ROOM_TEMP_GOOD_LED, LOW);
  }

  if (distance < CLOSE_DISTANCE_CM) {
    digitalWrite(RED_LED, HIGH);
  } else {
    digitalWrite(RED_LED, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  // This is the most important part of the Blynk setup! It uses your WiFi info and auth token
  // to get your board connected to the internet and the Blynk server.
  Blynk.begin(auth, ssid, pass);
  
  u8g2.begin();
  u8g2.clearDisplay();
  
  sensors.begin();
  dht.begin();
  
  Wire.begin(21, 22);
  if (!particleSensor.begin(Wire)) {
    Serial.println("MAX30102 was not found. Please check your wiring!");
    // If the sensor isn't found, the code gets stuck here forever so you know something's wrong.
    while (1);
  }
  
  particleSensor.setup();
  pinMode(POWER_LED, OUTPUT);
  digitalWrite(POWER_LED, HIGH);

  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(ROOM_TEMP_GOOD_LED, OUTPUT);
  pinMode(HUMAN_BODY_BUZZER, OUTPUT);
}

void loop() {
  // We use `static unsigned long` here to create a timer without blocking the code.
  // This lets the program do other things while it waits for a second to pass.
  static unsigned long lastUpdate = 0;
  
  // This line is super important! You have to call it constantly so your board can
  // talk to the Blynk server and receive any commands or updates.
  Blynk.run();
  
  particleSensor.check();
  
  long irValue = particleSensor.getIR();
  long redValue = particleSensor.getRed();
  
  // This simple check looks at the infrared light level to see if a finger is placed on the sensor.
  if (irValue > 50000) {
    fingerStatus = 1;

    // This is how we keep a "rolling window" of data. We're shifting all the old samples
    // down one spot in the array to make room for the newest sample at the front.
    // This way, we always have a recent set of 100 readings to work with.
    for(byte j=99; j>0; j--) {
      irArray[j] = irArray[j-1];
      redArray[j] = redArray[j-1];
    }
    
    // Add the most recent sample to the beginning of our data arrays.
    irArray[0] = irValue;
    redArray[0] = redValue;

    // We only start calculating the BPM and SpO2 once we've collected 100 samples.
    // That gives us enough data for a reliable reading.
    if(i < 99) {
      i++;
    } else {
      // This is the core of the pulse oximeter. We're calculating the AC and DC components of
      // the red and infrared light signals.
      // Think of the DC part as the "average brightness" of the light shining through your finger.
      // The AC part is the tiny "flicker" that happens with each heartbeat.
      float irAC = 0, irDC = 0, redAC = 0, redDC = 0;
      float irMax = 0, irMin = 1000000;
      float redMax = 0, redMin = 1000000;

      for (byte j = 0; j < 100; j++) {
        irDC += irArray[j];
        redDC += redArray[j];
        if (irArray[j] > irMax) irMax = irArray[j];
        if (irArray[j] < irMin) irMin = irArray[j];
        if (redArray[j] > redMax) redMax = redArray[j];
        if (redArray[j] < redMin) redMin = redArray[j];
      }
      irDC /= 100;
      redDC /= 100;
      
      irAC = irMax - irMin;
      redAC = redMax - redMin;

      // The SpO2 calculation uses a special formula based on the ratio of the pulsating (AC)
      // signals. It's how the sensor figures out your blood oxygen level.
      spo2_ratio = (redAC / redDC) / (irAC / irDC);

      // We're using a simple lookup table to convert the calculated ratio into a
      // percentage that you can actually understand.
      if (spo2_ratio > 1.7) {
        spo2 = 0;
      } else if (spo2_ratio > 1.2) {
        spo2 = 100 - (spo2_ratio - 1.2) * 50;
      } else {
        spo2 = 98 - (spo2_ratio - 0.4) * 20;
      }
      
      // To get the heart rate, we count how many times the infrared signal "pulsates" or
      // crosses a certain threshold over a specific period.
      int pulseCount = 0;
      for (byte j = 1; j < 100; j++) {
        // This condition looks for the rising edge of a pulse.
        if (irArray[j] > (irMin + irAC / 2) && irArray[j-1] < (irMin + irAC / 2)) {
          pulseCount++;
        }
      }
      // This formula takes the pulse count and converts it into a beats per minute (BPM) value.
      bpm = pulseCount * 60.0 / (1.0 * (millis() - lastUpdate) / 1000.0);
      
      // Safety check: if the reading is unrealistically high, we assume it's an error
      // and reset the BPM to zero.
      if (bpm > 220) {
          bpm = 0.0;
      }
    }
  } else {
    // If no finger is on the sensor, we reset everything so we can start fresh.
    fingerStatus = 0;
    bpm = 0.0;
    spo2 = 0.0;
    i = 0;
    for(byte j=0; j<100; j++) {
      irArray[j] = 0;
      redArray[j] = 0;
    }
  }

  // This check makes sure we only update the sensors, screen, and Blynk once per second,
  // which keeps the data stable and prevents us from spamming the system.
  if (millis() - lastUpdate > 1000) {
    readOtherSensors();
    controlIndicators();
    updateOLED();
    lastUpdate = millis();

    // These lines send the latest sensor data to your Blynk app's virtual pins.
    Blynk.virtualWrite(V2, humanTemp);
    Blynk.virtualWrite(V3, roomTemp);
    Blynk.virtualWrite(V4, humidity);
    Blynk.virtualWrite(V0, bpm);
    Blynk.virtualWrite(V1, spo2);

    // This is a neat way to log events in your Blynk app so you can see a history
    // of your measurements.
    Blynk.logEvent("body_temperature", String("Body Temp: ") + humanTemp + " C");
    Blynk.logEvent("heart_rate", String("Heart Rate: ") + bpm + " BPM");
    Blynk.logEvent("spo2", String("SpO2: ") + spo2 + " %");
  }
}```
