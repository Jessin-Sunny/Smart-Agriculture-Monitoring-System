//SMART AGRICULTURE SYSTEM
//LCD Screen libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>  // DHT Sensor Library

//initialize LCD Screen
LiquidCrystal_I2C lcd(0x27, 16, 2);   // 16 Columns and 2 rows

// Pin Definitions
const int soilMoisturePin = A0;   // HW-103 analog output
const int fireLedPin = D0;        // Fire Status as RED LED
const int buzzerPin = D8;         // adding buzzer
const int waterLedPin = D4;       // Excess Water indicator as GREEN LED
const int B1Pin = D5;          // Controls motor driver B1 Pin
const int A1Pin = D7;          // A1 Pin
#define DHTPin D6
#define DHTTYPE DHT11 
DHT dht(DHTPin, DHTTYPE);

const int dryVal = 750;   // ADC reading considered dry for irrigation
const int wetVal = 650;   // ADC reading considered for excess water


void setup() {
  Serial.begin(115200);           // Initialize Serial Monitor

  pinMode(soilMoisturePin, INPUT);
  pinMode(fireLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);   // Optional buzzer setup
  pinMode(waterLedPin, OUTPUT);
  pinMode(B1Pin, OUTPUT);
  pinMode(A1Pin, OUTPUT);

  lcd.init();        // Initialize LCD
  lcd.backlight();   // Turn on backlight
  lcd.clear();        //clear LCD

  digitalWrite(fireLedPin, LOW);
  digitalWrite(buzzerPin, LOW);
  digitalWrite(waterLedPin, LOW);
  dht.begin(); //  DHT sensor intialization
  digitalWrite(B1Pin, LOW); // Pump OFF initially
  digitalWrite(A1Pin, LOW);
}

void loop() {
  int moistureValue = analogRead(soilMoisturePin);  // Read soil moisture
  // Convert ADC to moisture %
  // Map the full ADC range to percentage
  // Invert logic: high ADC = dry → 0%, low ADC = wet → 100%
  int moisturePercent = map(moistureValue, 0, 1024, 100, 0);
  moisturePercent = constrain(moisturePercent, 0, 100);
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  float demoTemperature = temperature + 15;
  int fireTemperature = 50;
  String status = "";

  // Check for failed readings
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("DHT Sensor Error!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DHT Error");
  }
  
  // Fire Alert over Moisture alert [More Priority]
  if (demoTemperature >= fireTemperature) {
    status = "FIRE ALERT!!!";
    digitalWrite(fireLedPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T: ");
    lcd.print(demoTemperature);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print(status);
  }

  else {
    digitalWrite(fireLedPin, LOW);
    //LCD Status display
    // Optional: future buzzer logic
    if (moistureValue > dryVal) {
      status = "DRY: WATER IN";
      digitalWrite(B1Pin, HIGH); // Turn ON pump -> WATER IN
      digitalWrite(A1Pin, LOW);
      Serial.println("Relay ON");
      digitalWrite(waterLedPin, LOW);
      digitalWrite(buzzerPin, LOW);
      
    }
    else if (moistureValue < wetVal) {
      status = "OVER: WATER OUT";
      //digitalWrite(B1Pin, LOW); // Turn ON pump -> WATER OUT [opposite direction]
      //digitalWrite(A1Pin, HIGH);
      digitalWrite(B1Pin, LOW); // Pump OFF 
      digitalWrite(A1Pin, LOW);
      digitalWrite(waterLedPin, HIGH);
      digitalWrite(buzzerPin, HIGH);
    }
    else {
      status = "WET: FINE";
      digitalWrite(B1Pin, LOW); // Pump OFF 
      digitalWrite(A1Pin, LOW);
      Serial.println("Relay OFF");
      digitalWrite(waterLedPin, LOW);  // LED off
      digitalWrite(buzzerPin, LOW);
    }
    //output on LCD display
    lcd.clear();
    lcd.setCursor(0, 0);    //col,row
    lcd.print("M: ");
    lcd.print(moisturePercent);
    lcd.print("% ");
    lcd.print("T: ");
    lcd.print(temperature);
    lcd.print("C ");
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print(status);

  }
  
  //output on console
  Serial.print("ADC: ");
  Serial.print(moistureValue);
  Serial.print(" → Moisture: ");
  Serial.print(moisturePercent);
  Serial.print("% → ");
  Serial.println(status);
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print("°C, Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  
  delay(2000); // 2-second interval

}
