# Selfie-autonomous-car

*Selfie* is the student project of autonomous cars. Vehicles based on 1:10 scale RC cars are customized to be able to operate autonomously in simulated road environments. They are equipped with camera, computer vision computing unit, controller and set of sensors like magnetic encoders, distance sensors and IMU.

## Version 2.0 - *Selfie* 2.0

<img align="right" height="400" src="https://user-images.githubusercontent.com/16399071/44856111-e7c9af00-ac6c-11e8-8f9c-d272c861cc84.PNG">

Software released as version 2.0 was onboarded in the brand new vehicle built for International Autonomous Robot Racing Competition ([IARRC](https://www.robotcompetition.ca/iarrc2018/)) 2018 in Waterloo, Canada. *Selfie* won first prize outperforming in both Drag and Circut Races. The vehicle was able to reach the speed of 12 m/s in stable Drag Race. Picture presents the car in development (without bodywork).

The IARRC competiton consists of project presentation part (written report, video, oral presentation) and racing part. Racing events are running outdoors. Track is marked with white and yellow lines and cones. In *Drag Race* cars are competing on 60 meters of straight track. In *Curcuit Race* cars have to drive about 1000 meters (3 laps of about 330 meters).

The main challenges to overcome were start up light detection, high-speed line detection and trajectory corrections, collision avoidance and  varying lumininance conditions (areas covered with sun light or fully or partially shadowed).


### Description of content

#### cpp-opencv-app
two apps for various tasks, placed respectively in /CIRCUIT/ and /DRAG/ directories. Used to run at platform equipped with Intel I3, mostly using opencv

#### stm32-atollic
Vehicle controller application for FreeRTOS (Real-Time Operating System). Application designed to run on STM32F7 (AnyFC F7 board).

