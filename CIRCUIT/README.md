## C++ OpenCV App
This directory contains Computer Vision and supplementary applications. System was tested on Ubuntu16.04.4 witch OpenCV 3.4.1 library.

### opencv-app
Extracts requiered features from given frame. The output are trajectory of detected lane and info about specyfic events that occures in the environment.

### trajectory-planning
Gets input data from "opencv-app" and optimizes trajectory of the vehicle. 

### watchdog
Detects any other app malfunctions. If necessary it can restart other programs.

### Those lines should be included to startup script:
```
$ sudo /home/ubuntu/Desktop/Selfie-autonomous-car/cpp-opencv-app/watchdog/build/watchdog
$ sudo /home/ubuntu/Desktop/Selfie-autonomous-car/cpp-opencv-app/opencv-app/build/opencv-app
$ sudo /home/ubuntu/Desktop/Selfie-autonomous-car/cpp-opencv-app/trajectory-planning/build/trajectory-planning
```
