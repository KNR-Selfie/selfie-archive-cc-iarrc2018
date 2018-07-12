#!/bin/bash

pkill -f trajectory
pkill -f opencv

cd ~

ipcrm -M 50001
ipcrm -M 50002
ipcrm -M 50003
ipcrm -M 50004

gnome-terminal --geometry 20x35+0+0 -x sh -c "/home/selfie/Desktop/Selfie_APP/CIRCUIT/opencv-app/build/opencv-app; bash"

sleep 0.5

gnome-terminal --geometry 20x35+0+0 -x sh -c "/home/selfie/Desktop/Selfie_APP/CIRCUIT/trajectory-planning/build/trajectory-planning; bash"
