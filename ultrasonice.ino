#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Ultrasonic sensor pins
const int trigPin = 9;
const int echoPin = 10;

const int BUZZER = 11;

// Variables for distance measurement
long duration;
double fake_distance;
double distance;
unsigned long startMillis;

const int numberOfSamples = 31; // big data set and odd number to determine the MEDIAN
double distance_values[numberOfSamples];

// Set the LCD I2C address (0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Initialize ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(BUZZER, OUTPUT);

  lcd.init();         // Initialize the LCD
  lcd.backlight();    // Turn on the backlight
  lcd.setCursor(0, 0);

  startMillis = millis();
}

void loop() {

  fake_distance = getDistance();
  distance = getMedian();
  distance = sqrt(sq(distance) - sq(4));

  unsigned long beepInterval = map(distance, 5, 50, 500, 3000); //Closer = higher pitch

  // Buzzer beep
  if (millis() - startMillis >= beepInterval) {
    startMillis = millis();
    tone(BUZZER, 1000, 50); // 50ms beep
  
  //LCD logic
  static double previousDistance = -1;
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

  double sum = 0;

  for (int i = 0; i < numberOfSamples; i++) {
      // Send ultrasonic pulse
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);

      // Measure distance
      duration = pulseIn(echoPin, HIGH, 25000); // 25ms time-out to avoid long waits
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

double getMedian() {

  // Quick Sort - Used to sort the dataset
  quicksort(distance_values, 0, numberOfSamples - 1);

  // Find the Median
  return distance_values[(int)(numberOfSamples / 2)];
}

// Quick Sort function
void quicksort(double arr[], int low, int high) {
  if (low < high) {
    int pi = partition(arr, low, high); //pi = pivot
    quicksort(arr, low, pi - 1);
    quicksort(arr, pi + 1, high);
  }
}

// Partition function for Quick Sort
int partition(double arr[], int low, int high) {
  double pivot = arr[high];  // pivot element
  int i = (low - 1);  // index of smaller element
  
  for (int j = low; j < high; j++) {
    if (arr[j] <= pivot) {
      i++;
      swap(&arr[i], &arr[j]);
    }
  }
  swap(&arr[i + 1], &arr[high]);
  return (i + 1);
}

// Swap function to exchange values using pointers
void swap(double* a, double* b) {
  double temp = *a;
  *a = *b;
  *b = temp;
}
