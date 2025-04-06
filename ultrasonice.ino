#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Ultrasonic sensor pins
const int trigPin = 10;
const int echoPin = 11;

const int BUZZER = 9;

// LED pins
const int L1 = 2;
const int L2 = 3;
const int L3 = 4;
const int L4 = 5;
const int L5 = 6;

int leds[] = {L1, L2, L3, L4, L5};

// Variables for distance measurement
long duration;
double distance;
int numLeds;
unsigned long startMillis;

const int numberOfSamples = 30;
double distance_values[numberOfSamples];

// Set the LCD I2C address (0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Initialize ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize LED pins
  for (int i = 0; i < 5; i++) {
    pinMode(leds[i], OUTPUT);
  }

  pinMode(BUZZER, OUTPUT);

  lcd.init();         // Initialize the LCD
  lcd.backlight();    // Turn on the backlight
  lcd.setCursor(0, 0);


  startMillis = millis();

  Serial.begin(9600);
}

void loop() {

  distance = getDistance();

  // Map the distance to LED count and beep speed
  numLeds = map(distance, 2, 30, 1, 5);
  numLeds = constrain(numLeds, 0, 5);
  unsigned long beepInterval = map(distance, 5, 50, 500, 4000);

  
  // Turn on LEDs depending on distance
  for (int i = 0; i < 5; i++) {
    digitalWrite(leds[i], (i < numLeds) ? HIGH : LOW);
  }

  // Buzzer beep
  if (millis() - startMillis >= beepInterval) {
    startMillis = millis();
    tone(BUZZER, 1000, 50); // 50ms beep
    
    // Print distance
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm - LEDs: ");
  Serial.println(numLeds);
  
  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  lcd.print((double)distance, 2);
  lcd.print(" cm    ");
  }

  

  delay(25);
}

double getDistance() {

  double sum = 0;

  for (int i = 0; i < numberOfSamples ; i ++) {
      // Send ultrasonic pulse
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);

      // Measure distance
      duration = pulseIn(echoPin, HIGH, 25000); //25ms time-out to avoid long waits
      distance = duration * 0.034 / 2;
      if (distance > 2 && distance < 400) {
        distance_values[i] = distance;
        sum += distance;
      }
      else {
        distance_values[i] = 0;
      }
    }
    
    return sum / numberOfSamples;

}
