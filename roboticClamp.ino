#include <ESP32Servo.h>
// Define pin numbers
#define PIN_SG90_1 13
#define PIN_SG90_2 14
#define PIN_SG90_3 12
#define PIN_SG90_4 27
#define CAPACITIVE_SENSOR_PIN 36
#define BUTTON_SAVE_PIN 25
#define BUTTON_RESTORE_PIN 26

// Set pin number
int xPin = 32;   // VRX
int yPin = 33;   // VRY
int xPin_2 = 34;   // VRX
int yPin_2 = 35;   // VRY

int servoY = 0;
int servoX = 90;
int servoZ = 60;
int servoA = 90;

// Variables to store saved positions
int savedServoY = 0;
int savedServoX = 90;
int savedServoZ = 60;
int savedServoA = 90;

int i = 0;
int j = 0;

bool armActive = true; // Track on/off condition of robot
bool lastSensorState = LOW; // Last capacitive sensor state
unsigned long lastDebounceTime = 0; // Last time the output pin was toggled
unsigned long debounceDelay = 50; // Debounce time

const unsigned long AUTO_MODE_HOLD_DURATION = 2000; // Duration (ms) to hold the sensor for auto mode
bool autoMode = false; // Track if the arm is in auto mode
unsigned long sensorPressStart = 0; // Track the sensor when it was pressed

const unsigned long HOLD_DURATION = 1000; // Duration (ms) to hold the button

int servoZlimit;  // Servo degree limit
int servoYlimit;

int sensorValue;

bool autoModeTemp;

Servo sg90_1;
Servo sg90_2;
Servo sg90_3;
Servo sg90_4;

void moveServoGradually(Servo &servo, int targetPosition, int stepDelay = 15) {
    int currentPosition = servo.read(); // Get the current position of the servo
    if (currentPosition < targetPosition) {
        for (int pos = currentPosition; pos <= targetPosition; pos++) {
            servo.write(pos); // Move to the next position
            delay(stepDelay); // Wait for a short time
        }
    } else {
        for (int pos = currentPosition; pos >= targetPosition; pos--) {
            servo.write(pos); // Move to the next position
            delay(stepDelay); // Wait for a short time
        }
    }
}

void autoModeMovement() {
  while(autoMode){  // Hardcoded auto mode movements
    moveServoGradually(sg90_2, 117);
    moveServoGradually(sg90_1, 69);
    moveServoGradually(sg90_4, 90);
    moveServoGradually(sg90_3, 86);
    moveServoGradually(sg90_4, 5);   //A
    
    delay(1000);

    if (!autoMode){   // To check if the user exits auto mode
      moveServoGradually(sg90_2, savedServoX);
      moveServoGradually(sg90_1, savedServoY);
      moveServoGradually(sg90_3, savedServoZ);
      moveServoGradually(sg90_4, savedServoA);
      break;
    }
    moveServoGradually(sg90_1, 22);   //Y
    moveServoGradually(sg90_3, 115);  //Z
    moveServoGradually(sg90_2, 92);   //X

    moveServoGradually(sg90_2, 65);   //X
    moveServoGradually(sg90_3, 78);  //Z
    moveServoGradually(sg90_1, 74);   //Y
    delay(1000);

    if (!autoMode){
      moveServoGradually(sg90_2, savedServoX);
      moveServoGradually(sg90_1, savedServoY);
      moveServoGradually(sg90_3, savedServoZ);
      moveServoGradually(sg90_4, savedServoA);
      break;
    }

    moveServoGradually(sg90_4, 90);   //A
    delay(1000);
    moveServoGradually(sg90_4, 5);   //A
    delay(1000);

    if (!autoMode){
      moveServoGradually(sg90_2, savedServoX);
      moveServoGradually(sg90_1, savedServoY);
      moveServoGradually(sg90_3, savedServoZ);
      moveServoGradually(sg90_4, savedServoA);
      break;
    }

    moveServoGradually(sg90_1, 22);   //Y
    moveServoGradually(sg90_3, 115);  //Z
    moveServoGradually(sg90_2, 92);   //X

    moveServoGradually(sg90_2, 117);
    moveServoGradually(sg90_3, 86);
    moveServoGradually(sg90_1, 69);
    moveServoGradually(sg90_4, 90);
    delay(1000);

    if (!autoMode){
      moveServoGradually(sg90_2, savedServoX);
      moveServoGradually(sg90_1, savedServoY);
      moveServoGradually(sg90_3, savedServoZ);
      moveServoGradually(sg90_4, savedServoA);
      break;
    }
  }
  if (!autoMode){
      moveServoGradually(sg90_2, savedServoX);
      moveServoGradually(sg90_1, savedServoY);
      moveServoGradually(sg90_3, savedServoZ);
      moveServoGradually(sg90_4, savedServoA);
  }
}

volatile bool sensorPressed = false; // Flag to indicate sensor press

// Interrupt Service Routine
void IRAM_ATTR handleSensorPress() {
  autoMode = false;
  autoModeTemp = false; // Some garbage variables havn't cleared
  //Serial.print("automode -> false."); // Debug line
  detachInterrupt(digitalPinToInterrupt(CAPACITIVE_SENSOR_PIN));
}

void setup() {
  Serial.begin(115200);

  pinMode(CAPACITIVE_SENSOR_PIN, INPUT); // Set capacitive sensor pin as input
  pinMode(BUTTON_SAVE_PIN, INPUT_PULLUP); // Set save button as input with pull-up
  pinMode(BUTTON_RESTORE_PIN, INPUT_PULLUP); // Set restore button as input with pull-up

  sg90_1.setPeriodHertz(50); // PWM frequency for SG90
  sg90_1.attach(PIN_SG90_1, 500, 2400); // Minimum and maximum pulse width in µs (currently not 180 degrees)
  sg90_2.setPeriodHertz(50);
  sg90_2.attach(PIN_SG90_2, 500, 2400);
  sg90_3.setPeriodHertz(50);
  sg90_3.attach(PIN_SG90_3, 500, 2400);
  sg90_4.setPeriodHertz(50);
  sg90_4.attach(PIN_SG90_4, 500, 2400);
}

void loop() {

  if (!autoMode) {
    sensorValue = digitalRead(CAPACITIVE_SENSOR_PIN);
    
    // Check for sensor activation (with debounce)
    if (sensorValue == HIGH) {
        if (lastSensorState == LOW) {
            // Sensor activated     
            sensorPressStart = millis(); // Record the start time
        }
        // Check if the sensor has been held long enough for auto mode
        if (millis() - sensorPressStart >= AUTO_MODE_HOLD_DURATION && !autoMode) {
            autoModeTemp = true; // Activate auto mode
        }
    } else {
        // Sensor is released
        if (lastSensorState == HIGH) {
          if (autoModeTemp){
            autoMode = true;
          } else{
            armActive = !armActive;
          }
        }
    }
    lastSensorState = sensorValue; // Save the sensor state for the next loop
  } 

  // If in auto mode, call the auto movement function
  if (armActive && autoMode) {
    attachInterrupt(digitalPinToInterrupt(CAPACITIVE_SENSOR_PIN), handleSensorPress, RISING);
    autoModeMovement();
  }

  if (armActive) {
  int xValue = analogRead(xPin);  // Read the X-axis value for joystick
  int yValue = analogRead(yPin);
  int xValue2 = analogRead(xPin_2);
  int yValue2 = analogRead(yPin_2);

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

  if (vx == -100 && servoY < servoYlimit){  // Some sort of unoptimized block of code for the servo motor control logic
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

  if (vy == 100 && servoX < 130){   // Hardcoded limit value for servoX
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

  // Check the state of the save button
  if (digitalRead(BUTTON_SAVE_PIN) == LOW) {
      unsigned long buttonPressStart = millis(); // Record the start time
      while (digitalRead(BUTTON_SAVE_PIN) == LOW) {
          // Wait until the button is released
          if (millis() - buttonPressStart >= HOLD_DURATION) {
              // Button has been held long enough to trigger save
              savedServoY = servoY;
              savedServoX = servoX;
              savedServoZ = servoZ;
              savedServoA = servoA;
              break;
          }
      }
      delay(500); // Debounce delay
  }
  // Check if the restore button is pressed
  if (digitalRead(BUTTON_RESTORE_PIN) == LOW) {
      moveServoGradually(sg90_2, savedServoX);
      moveServoGradually(sg90_1, savedServoY);
      moveServoGradually(sg90_3, savedServoZ);
      moveServoGradually(sg90_4, savedServoA);
      servoY = savedServoY;
      servoX = savedServoX;
      servoZ = savedServoZ;
      servoA = savedServoA;
      delay(500); // Debounce delay
  }


  // Print vx and vy on separate lines
  //Serial.print("X: ");
  /*Serial.print(servoX);
  Serial.print(F(","));
  Serial.print(servoY);
  Serial.print(F(","));
  Serial.print(servoZ);
  Serial.print(F(","));
  //Serial.print("\tY: ");
  Serial.println(servoA);*/ // Some debug lines
  
  sg90_1.write(servoY);
  sg90_2.write(servoX);
  sg90_3.write(servoZ);
  sg90_4.write(servoA);

  delay(10); // Value can be adjusted
  }
}
