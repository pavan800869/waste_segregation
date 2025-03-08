# ESP32-CAM Waste Segregation System

## 📌 Overview
This project uses an **ESP32-CAM** with an **Edge Impulse object detection model** to classify waste as either **dry waste** or **wet waste**. Based on the classification, the system rotates a servo motor to direct the waste into the appropriate bin.

## 🚀 Features
- **Real-time waste classification** using Edge Impulse AI model.
- **ESP32-CAM integration** for capturing and processing images.
- **Automated bin segregation** with two servo motors:
  - Rotates **left** for dry waste.
  - Rotates **right** for wet waste.
- **Lightweight and efficient** model for edge computing.

## 🛠️ Hardware Requirements
- ESP32-CAM  
- Two servo motors (one for dry waste, one for wet waste)  
- Power supply (5V for ESP32-CAM)  
- Jumper wires  

## 📦 Software Requirements
- Arduino IDE with ESP32 board support  
- Edge Impulse SDK for ESP32  
- `ESP32Servo.h` library for servo control  
- `tensorflowlite_esp32` for AI inference  

## 🔧 Setup & Installation
1. **Clone this repository**  
   ```sh
   git clone https://github.com/pavan800869/waste_segregation.git
   cd esp32-cam-waste-segregation
