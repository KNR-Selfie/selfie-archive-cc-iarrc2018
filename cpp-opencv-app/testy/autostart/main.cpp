#include <iostream>
#include <string>
#include <cstdlib>
#include <stdlib.h>
#include <unistd.h>
#include <thread>

int main(int argc, char** argv)
{
    //Start new terminal with watchdog
    system("gnome-terminal -x sh -c \"/home/ubuntu/Desktop/SELFIE_watchdog/watchdog/build/watchdog; bash\"");

    std::this_thread::sleep_for(std::chrono::seconds(1));

    //Start new terminal with app
    system("gnome-terminal -x sh -c \"/home/ubuntu/Desktop/SELFIE_watchdog/app/build/app; bash\"");

    return 0;
}
