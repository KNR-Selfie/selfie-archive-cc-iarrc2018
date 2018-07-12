#!/bin/bash

pkill -f trajectory
pkill -f opencv

cd ~

ipcrm -M 50001
ipcrm -M 50002
ipcrm -M 50003
ipcrm -M 50004

gnome-terminal --geometry 20x35+0+0 -x sh -c "Desktop/Selfie-autonomous-car/DRAG/multiple-ROIs-lane-detection/build/multiple-ROIs-lane-detection; bash"

sleep 0.5

gnome-terminal --geometry 20x35+0+0 -x sh -c "Desktop/Selfie-autonomous-car/DRAG/trajectory-planning/build/trajectory-planning; bash"
