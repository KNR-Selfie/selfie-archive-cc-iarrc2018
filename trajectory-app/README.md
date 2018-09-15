# Selfie-autonomous-car

*Selfie* is the student project of autonomous cars. Vehicles based on 1:10 scale RC cars are customized to be able to operate autonomously in simulated road environments. They are equipped with camera, computer vision computing unit, controller and set of sensors like magnetic encoders, distance sensors and IMU.

This file contains only basic informations, more details will hopefully occur as Wiki in finite time :)

## Version 1.0 - *Selfie*

<img align="right" height="300" src="https://user-images.githubusercontent.com/16399071/37375786-96f3a8d6-2721-11e8-87de-6ebe179637dd.JPG">

Software released as [version 1.0](https://github.com/perciax/Selfie-autonomous-car/tree/v1.0) in first ready-to-go robot *Selfie*. It was prepared specifically for [Carolo-Cup 2018](https://wiki.ifr.ing.tu-bs.de/carolocup/) competition. It is fullfilling all the competition requirements, so basically it is able to perform:
- road lane tracking, 
- collision avoiding,
- overtaking maneuver
- parallel parking
- intersection handling
- proper light signalization

### Description of content

#### cpp-opencv-app
Computer Vision application designed to run on NVidia Jetson / Odroid platform.

#### stm32-atollic
Vehicle controller application for FreeRTOS (Real-Time Operating System). Aplication is designed to run on STM32F7 (AnyFC F7 board)

## How to contribute

1. Fork this repo to your account.

2. Clone repo from your account to local folder on your device.

	`git clone https://github.com/your-login/Selfie-autonomous-car`

3. Add new remote repo as "main".

	`git remote add main https://github.com/perciax/Selfie-autonomous-car`

Now you should have 2 remote repos configured:
- "main" pointing this repo,
- "origin" pointing copy on you account.
This could be checked by command git remote -v.

4. Update your local verison.

	`git pull main master`

5. Make your changes.

6. Push them to repo on you account.

	`git push origin master`

7. Make Pull Request on Github.

8. Repeat steps from 4 to 7 everytime you want to contribute to this "main" repo.	
