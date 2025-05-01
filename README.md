# ESP32 Servo Control Project for 3-DOF Robotic Clamp

## Overview
This project uses an ESP32 board to control a 3-DOF robotic clamp made from cardboard. The clamp is operated using multiple SG90 servos, which can be controlled via capacitive sensors and joystick modules. The system supports both manual and automatic modes.

## Features
- Control up to four SG90 servos for the robotic clamp.
- Capacitive sensor for turning the robotic clamp on/off as well as activating automatic mode.
- Joystick input for manual control of servo positions.
- Save and restore servo positions using buttons.

## Components Used
- ESP32 WROVER IE Module
- 4 x SG90/MG90 Servo Motors
- 2 x Joystick Modules
- 1 x Capacitive Touch Sensor
- Cardboard (for constructing the robotic clamp)
- Jumper Wires
- Breadboard

## Circuit Connections

(Circuit Diagrams and Pin Connections would be updated afterwards.)

## Installation

1. **Install Required Libraries**:
   Install the `ESP32Servo` library via the Arduino Library Manager:
   - Open Arduino IDE
   - Go to **Sketch** > **Include Library** > **Manage Libraries**
   - Search for `ESP32Servo` and install.

2. **Upload the Code**:
   Open the `.ino` file in Arduino IDE and upload it to your ESP32 board.

## Usage
- **Manual Control**: Use the joystick modules to control the servos. The X and Y axes of each joystick correspond to different servos.
- **Automatic Mode**: Press and hold the capacitive sensor to activate automatic mode. The servos will move in a predefined sequence.
- **Save Positions**: Press the save button to store the current positions of all the servos.
- **Restore Positions**: Press the restore button to return the servos to the saved positions.

## Code Explanation
- The code initializes the servos and sets up the input pins.
- The `moveServoGradually` function allows for transitions between servo positions, so that the clamp does not 'fly' from point to point.
- The `autoModeMovement` function defines the sequence of movements for the servos in automatic mode.
- The main loop checks for input from the capacitive sensor and joystick modules to control the servos accordingly.

## Acknowledgments
- Arduino community for support and resources.
- ChatGPT for assistance in project documentation, code explanations and debugging.
