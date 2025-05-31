#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Ultrasonic sensor pins
const int trigPin = 9;
const int echoPin = 10;
const int BUZZER = 11;

// Variables for distance measurement
long duration;
double distance;
unsigned long startMillis;
unsigned long lcdMillis;

const int numberOfSamples = 31; 
double distance_values[numberOfSamples];

// Set the LCD I2C address (0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);
//SDA PIN A4
//SCL PIN A5

void setup() {
  // Initialize ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(BUZZER, OUTPUT);

  lcd.init();         
  lcd.backlight();    
  lcd.setCursor(0, 0);
  lcd.print("Measuring...");

  startMillis = millis();
  lcdMillis = millis();
}

void loop() {

  getDistance(); //populates distance_values[]
  distance = getMean();
  //distance = sqrt(sq(distance) - sq(4));

  unsigned long beepInterval = map(distance, 5, 50, 500, 3000); //Closer = higher pitch

  // Buzzer beep
  if (millis() - startMillis >= beepInterval) {
    startMillis = millis();
    tone(BUZZER, 1000, 50); // 50ms beep
  }

  //LCD logic
  static double previousDistance = -1;

  if (millis() - lcdMillis >= 500){
    lcdMillis = millis();
    if (abs(distance - previousDistance ) > 0.1){
      previousDistance = distance;
      lcd.setCursor(0, 0);
      lcd.print("Dist: ");
      lcd.print((double)distance, 2);
      lcd.print(" cm    ");
    }
  }
  
  delay(25);
}

double getDistance() {
  int validSamples = 0;
  int attempts = 0;
  const int maxAttempts = numberOfSamples * 3; // up to 3 tries per sample
  double lastGood = -1;

  while (validSamples < numberOfSamples && attempts < maxAttempts) {
    attempts++;

    // Trigger pulse
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Read echo
    duration = pulseIn(echoPin, HIGH, 25000); // ~50 cm max
    double dist = duration * 0.034 / 2;

    // Validate reading
    if (dist >= 2 && dist <= 400) { // 2cm - 4m
      if (lastGood < 0 || abs(dist - lastGood) <= 2) {
        distance_values[validSamples++] = dist;
        lastGood = dist;
      }
    }
  }

  // If not enough samples collected, fall back to average of what we have
  if (validSamples < numberOfSamples) {
    for (int i = validSamples; i < numberOfSamples; i++) {
      distance_values[i] = lastGood;
    }
  }

  return validSamples > 0 ? lastGood : 0;
}

double getMean() {
  double sum = 0;
  for (int i = 0; i < numberOfSamples; i++) {
    sum = sum + distance_values[i];
  }

  return sum / numberOfSamples;
}

