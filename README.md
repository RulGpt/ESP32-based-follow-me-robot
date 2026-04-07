# ESP32-based-follow-me-robot
AI-powered ESP32 robot that streams live video, detects and tracks a person using computer vision, and autonomously follows them via real-time motor control. Uses Python, OpenCV, and lightweight models for smooth, responsive navigation over Wi-Fi.



🤖 AI-Based Human Following Robot (ESP32 + Computer Vision)
📌 Overview
This project is an AI-powered autonomous robot that detects and follows a human in real time using computer vision. The system uses an ESP32-CAM for video streaming and an ESP32 Dev Module for motion control, while a Python script performs object detection and decision-making.
The robot dynamically adjusts its movement (forward, backward, left, right) based on the position and distance of a detected person.

🎯 Key Features
🎥 Live video streaming using ESP32-CAM
🧠 Real-time human detection using YOLOv8
🚶 Autonomous human-following behavior
⚡ Dynamic motion control (speed + duration based)
📡 Wi-Fi-based communication between Python and ESP32
🔄 Non-blocking motor control (smooth operation)
🧩 Modular architecture (easy to upgrade models/hardware)

🏗️ System Architecture

ESP32-CAM  →  WiFi Stream  →  Python (YOLO + Logic)  →  HTTP Commands  →  ESP32 Controller → Motors

Components:
ESP32-CAM
     Streams video over HTTP
     No processing load

Python Script
     Receives video stream
     Runs object detection (YOLOv8)
     Calculates position + distance
     Sends movement commands

ESP32 Dev Module
     Hosts web server
     Receives commands
     Controls motors via L293D


🧰 Tech Stack
Hardware:
     ESP32-CAM (AI Thinker)
     ESP32 Dev Module
     L293D Motor Driver
     DC Motors
     9V Batteries + Voltage Regulator (LM7805)

Software:
     Python 3.10+
     OpenCV
     Ultralytics YOLOv8
     Requests (HTTP communication)
     Arduino IDE (ESP32 firmware)
     
     
⚙️ How It Works
1. Video Streaming
ESP32-CAM streams MJPEG video:
      http://<ESP32-CAM-IP>:81/stream
   
2. Object Detection
     Python script:
               Captures frames
               Runs YOLO model
               Filters only person class (class 0)

3. Decision Logic
Based on:
X position (left/right alignment) and Bounding box area (distance estimation)
   Condition       Action
Person left       Turn Left
Person right      Turn Right
Too far           Move Forward
Too close         Move Backward
Ideal distance    Stop


4. Smart Movement Control
Instead of fixed movement, movement duration is dynamically calculated
Example:
     duration ∝ distance from target
   
This results in:
     smoother motion
     less jitter
     better tracking

6. Communication
Python → ESP32 via HTTP:  -----> http://<ESP32-IP>/cmd?val=F&dur=300
Where:
val = Command (F, B, L, R, S)
dur = Movement duration (ms)
