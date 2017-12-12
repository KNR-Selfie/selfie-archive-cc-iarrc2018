#include <iostream>
#include <string>
#include <cstdlib>
#include <stdlib.h>
#include <unistd.h>
#include <thread>

int main(int argc, char** argv)
{
    //Start new terminal with watchdog
    system("gnome-terminal --geometry 26x7-0+0 -x sh -c \"/home/odroid/Desktop/opencv-app/watchdog/build/watchdog; bash\"");

    return 0;
}
