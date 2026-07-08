#include <Wire.h> // Biblioteca pentru comunicare I2C
#include <LiquidCrystal_I2C.h> // Biblioteca LCD cu interfata I2C
#include "DHT.h" // Biblioteca pentru senzorul DHT22

// Definirea pinilor
#define DHTPIN 10
#define DHTTYPE DHT22
#define VOLTAGE_PIN A2
#define CURRENT_PIN A3

// Constante de conversie si referinta
const float VOLTAGE_REF = 5.0;
const float VOLTAGE_RATIO = 2.0;
const float CURRENT_SENSITIVITY  = 0.185;
const float ACS_OFFSET  = 2.5;

// Interval pentru refresh LCD
const unsigned long LCD_REFRESH_INTERVAL  = 1000;

// Obiecte pentru senzori si LCD
DHT  dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adresa I2C 0x27, LCD 16x2

// Variabile de stare
unsigned long lastLcdRefreshTime = 0;

// Functie de initializare
void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  // Initializam LCD-ul
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializare...");
  
  // Initializam senzorul DHT22
  dht.begin();
  
  // Asteptam stabilizarea
  delay(2000);
  lcd.clear();
}

// Functia principala - loop
void loop() {
  // Citim valorile senzorilor
  float temperature = dht.readTemperature();
  float voltage = readVoltage();
  float current = readCurrent();
  
  // Afisam valorile pe LCD
  updateDisplay(temperature, voltage, current);
  
  // Afisam si pe Serial pentru debug
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print("C, Tens: ");
  Serial.print(voltage);
  Serial.print("V, Curent: ");
  Serial.print(current * 1000);
  Serial.println("mA");
  delay(500);
}

// Citire tensiune de la divizor
float readVoltage() {
  int raw = 0;
  for (int i = 0; i < 5; i++) { 
    raw += analogRead(VOLTAGE_PIN); 
    delay(5);
  }
  return ((raw / 5) / 1023.0) * VOLTAGE_REF * VOLTAGE_RATIO;
}

// Citire curent de la ACS712
float readCurrent() {
  int raw = 0;
  for (int i = 0; i < 10; i++) { 
    raw += analogRead(CURRENT_PIN); 
    delay(2);
  }
  float voltage = (raw / 10) / 1023.0 * VOLTAGE_REF;
  return (voltage - ACS_OFFSET) / CURRENT_SENSITIVITY;
}

// Afiseaza toate valorile pe acelasi ecran LCD
void updateDisplay(float temperature, float voltage, float current) {
  // Actualizam doar la intervalul specificat
  if (millis() - lastLcdRefreshTime < LCD_REFRESH_INTERVAL) return;
  lastLcdRefreshTime = millis();
  
  // Prima linie: Temperatura si tensiune
  lcd.setCursor(0, 0);
  lcd.print("T:");
  if (!isnan(temperature)) {
    lcd.print(temperature, 1);
    lcd.print((char)223);
    lcd.print("C ");
  } else {
    lcd.print("Err ");
  }
  
  lcd.print("V:");
  lcd.print(voltage, 1);
  lcd.print("V");
  
  // A doua linie: Curent
  lcd.setCursor(0, 1);
  lcd.print("Curent:");
  lcd.print(current * 1000, 1); // Convertim la mA
  lcd.print("mA  ");
}