# Selfie-autonomous-car
Autonomous RC car project

## Description of content

### cpp-opencv-app (under development)
Computer Vision application designed to run on NVidia Jetson / Odroid platform.

### stm32-app (under development) 
Vehicle controller based on FreeRTOS. Project for STM32F7 (AnyFC F7 board)

### java-desktop-app (under development / suspended)
Java application for Desktop communicating via Bluetooth.

### RPi-robot (under development / suspended)
To run projects, on your RPi, have to fill remotedebug.xml files with your pi logging data and remove .dist extention.
Remote debuging done according to: http://www.raspirecipes.org/archives/218

### android-app (under development / suspended)
Application for Android System to handle vehicle parameters via Bluetooth

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
