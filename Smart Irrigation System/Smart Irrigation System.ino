#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

// WiFi credentials
const char* ssid = "Karthick";     
const char* password = "12345678"; 

// Firebase credentials
#define FIREBASE_HOST "https://sih-hardware-202d6-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "AIzaSyAHAOZM_W6CNmRbxBa1qCWIA4Gza9ysfa0"

// Soil moisture sensor pin
#define SOIL_MOISTURE_SENSOR_PIN 33  
#define RELAY_PIN 4                   
#define BUTTON_PIN 2                  
// LCD I2C wiring
const int I2C_SDA_PIN = 21;       
const int I2C_SCL_PIN = 22;       
const int I2C_ADDR = 0x27;        
const int LCD_COLUMNS = 16;       
const int LCD_ROWS = 2;          


LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_ROWS);
FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;


int moistureValue = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Soil Moisture and Relay Control");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  
  config.host = FIREBASE_HOST;
  config.api_key = FIREBASE_AUTH;


   auth.user.email = "agventure06@gmail.com";  
   auth.user.password = "agventure06";  


  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  
  digitalWrite(RELAY_PIN, HIGH); 
 
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  lcd.init();
  lcd.backlight();

 
  lcd.setCursor(0, 0);
  lcd.print("Soil Moisture");
  delay(3000);  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moisture:");
}

void loop() {
  
  moistureValue = analogRead(SOIL_MOISTURE_SENSOR_PIN);
  moistureValue = map(moistureValue, 0, 4095, 0, 100); 
  moistureValue = (moistureValue - 100) * -1; 

  
  if (moistureValue < 0) {
    moistureValue = 0;
  }

  
  Serial.print("Moisture: ");
  Serial.print(moistureValue);
  Serial.println("%");

  
  lcd.setCursor(0, 1);
  lcd.print("  "); 
  lcd.setCursor(0, 1);
  lcd.print(moistureValue);
  lcd.print("%");

  
  String path = "/moisture"; 
  FirebaseJson json;
  json.set("moistureValue", moistureValue);

  
  if (Firebase.updateNode(firebaseData, path, json)) {
    Serial.println("Data updated successfully");
  } else {
    Serial.println("Failed to update data");
    Serial.println("REASON: " + firebaseData.errorReason());
  }

  
  if (digitalRead(BUTTON_PIN) == LOW) {  
    digitalWrite(RELAY_PIN, LOW); 
    lcd.setCursor(0, 1);
    lcd.print("Relay ON     ");
  } else {
    digitalWrite(RELAY_PIN, HIGH); 
    lcd.setCursor(0, 1);
    lcd.print("Relay OFF    ");
  }

  delay(1000); 
}