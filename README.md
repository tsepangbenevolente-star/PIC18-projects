# PIC18 Projects Portfolio

This repository contains embedded systems projects developed using the PIC18F25K20 microcontroller.As a 2nd year student the goal of this portfolio is to demonstrate proficiency in embedded C, sensor data acquisition, and hardware logic as a foundational step toward advanced machine learning and hardware integration.



Project 1: Three-Zone Spatial Containment Shield

Overview
This project is an active perimeter security shield designed to protect sensitive laboratory environments. It utilizes ultrasonic and infrared (IR) proximity sensors to create three distinct concentric warning zones (Safe, Warning, Critical) around an object, providing dynamic visual and auditory feedback based on intrusion proximity.

Hardware & Tech Stack
* Microcontroller: PIC18F25K20
* Language: Embedded C
* Sensors: HC-SR04 Ultrasonic Distance Sensor, Mini PIR Motion Sensor
* Actuators/Outputs: Red & Green LEDs, Piezo Buzzer, SG90 Micro Servo Motor
* Display: 2x16 Blue LCD

# How It Works
The system continuously calculates distance using the ultrasonic sensor and categorizes the space into three zones:
1. Safe Zone (>100cm): System is idle. Green LED active.
2. Warning Zone (20cm - 100cm): Intrusion detected. Red LED active, buzzer emits a monotone warning pulse.
3. Critical Zone (<20cm): Immediate breach. Buzzer emits a dissonant, high-frequency warble to signal critical proximity.

# Repository Navigation
* `/Project1_Containment_Shield/src/` - Contains the main C source code.
* `/Project1_Containment_Shield/docs/` - Contains the full technical design report and Bill of Materials. 
