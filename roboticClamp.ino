#include <ESP32Servo.h>
#define PIN_SG90_1 13 // Output pin used
#define PIN_SG90_2 14 // Output pin used
#define PIN_SG90_3 12 // Output pin used
#define PIN_SG90_4 27 // Output pin used
#define CAPACITIVE_SENSOR_PIN 36 // Capacitive sensor pin

int xPin = 32;   // VRX pin
int yPin = 33;   // VRY pin
int xPin_2 = 34;   // VRX pin
int yPin_2 = 35;   // VRY pin
//int buttonPin = 32; // SW pin
int servoY = 0;
int servoX = 90;
int servoZ = 60;
int servoA = 90;

int i = 0;
int j = 0;

bool armActive = true; // Variable to track if the arm is active
bool lastSensorState = LOW; // Last capacitive sensor state
unsigned long lastDebounceTime = 0; // Last time the output pin was toggled
unsigned long debounceDelay = 50; // Debounce time

int servoZlimit;
int servoYlimit;

Servo sg90_1;
Servo sg90_2;
Servo sg90_3;
Servo sg90_4;

void setup() {
  Serial.begin(115200);
  //pinMode(buttonPin, INPUT_PULLUP); // Pull-up for the button
  pinMode(CAPACITIVE_SENSOR_PIN, INPUT); // Set capacitive sensor pin as input
  sg90_1.setPeriodHertz(50); // PWM frequency for SG90
  sg90_1.attach(PIN_SG90_1, 500, 2400); // Minimum and maximum pulse width (in µs) to go from 0° to 180
  sg90_2.setPeriodHertz(50); // PWM frequency for SG90
  sg90_2.attach(PIN_SG90_2, 500, 2400); // Minimum and maximum pulse width (in µs) to go from 0° to 180
  sg90_3.setPeriodHertz(50); // PWM frequency for SG90
  sg90_3.attach(PIN_SG90_3, 500, 2400); // Minimum and maximum pulse width (in µs) to go from 0° to 180
  sg90_4.setPeriodHertz(50); // PWM frequency for SG90
  sg90_4.attach(PIN_SG90_4, 500, 2400); // Minimum and maximum pulse width (in µs) to go from 0° to 180
}

void loop() {

  // Read the capacitive sensor state
  int sensorValue = digitalRead(CAPACITIVE_SENSOR_PIN);

  // Check for sensor activation (with debounce)
  if (sensorValue == HIGH && lastSensorState == LOW && (millis() - lastDebounceTime) > debounceDelay) {
      armActive = !armActive; // Toggle the arm's active state
      lastDebounceTime = millis(); // Update the last debounce time
  } else if (sensorValue == LOW && lastSensorState == HIGH) {
      // This condition allows toggling when the sensor is released
      lastDebounceTime = millis(); // Update the last debounce time
  }
  lastSensorState = sensorValue; // Save the sensor state for the next loop

  if (armActive) {
  int xValue = analogRead(xPin);  // Read the X-axis value
  int yValue = analogRead(yPin);  // Read the Y-axis value
  int xValue2 = analogRead(xPin_2);  // Read the X-axis value
  int yValue2 = analogRead(yPin_2);  // Read the Y-axis value

  // Map the values for better visualization in the Serial Plotter
  int vx = map(xValue, 0, 4095, -100, 100);
  int vy = map(yValue, 0, 4095, -100, 100);
  int vx2 = map(xValue2, 0, 4095, -100, 100);
  int vy2 = map(yValue2, 0, 4095, -100, 100);

  if (servoZ <= 95) {
    servoYlimit = 75; // If servoZ is 95 or less, set to 75
  } else if (servoZ >= 150) {
      servoYlimit = 35; // If servoZ is 150 or more, set to 35
  } else {
      // Linear interpolation between 95 and 150
      servoYlimit = 75 + (30 - 75) * (servoZ - 95) / (150 - 95);
  }

  if (vx == -100 && servoY < servoYlimit){
    if (i == 1){
      servoY++;
      i = 0;
    } else i++;
  };
  if (vx == 100 && servoY > 0){
    if (i == 1){
      servoY--;
      i = 0;
    } else i++;
  };

  if (vy == 100 && servoX < 130){
    if (j == 1){
      servoX++;
      j = 0;
    } else j++;
  };
  if (vy == -100 && servoX > 45){
    if (j == 1){
      servoX--;
      j = 0;
    } else j++;
  };


  if (servoY <= 35) {
    servoZlimit = 150;
  } else if (servoY >= 75) {
      servoZlimit = 90;
  } else {
      servoZlimit = 150 + (95 - 150) * (servoY - 30) / (75 - 30);
  }

  if (vx2 == -100 && servoZ < servoZlimit){
    servoZ++;
  };
  if (vx2 == 100 && servoZ > 60){
    servoZ--;
  };

  if (vy2 == -100 && servoA < 90){
    servoA++;
  };
  if (vy2 == 100 && servoA > 10){
    servoA--;
  };


  // Print vx and vy on separate lines
  //Serial.print("X: ");
  Serial.print(servoX);
  Serial.print(F(","));
  Serial.print(servoY);
  Serial.print(F(","));
  Serial.print(servoZ);
  Serial.print(F(","));
  //Serial.print("\tY: ");
  Serial.println(servoA); // Use println to send a new line for the plotter
  
  sg90_1.write(servoY);
  sg90_2.write(servoX);
  sg90_3.write(servoZ);
  sg90_4.write(servoA);

  delay(10); // Adjust delay as needed for smooth plotting
  }
}