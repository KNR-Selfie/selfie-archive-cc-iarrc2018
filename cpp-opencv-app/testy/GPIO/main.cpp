#include <iostream>
#include <string>
#include <unistd.h>
#include "include/GPIO.cpp"

using namespace std;

int main(int argc, char *argv[])
{
    jetsonGPIO redLED = gpio164 ;
    jetsonGPIO yellowLED = gpio165 ;
    jetsonGPIO greenLED = gpio166 ;

    // Make the button and led available in user space
    gpioExport(redLED);
    gpioSetDirection(redLED,outputPin);
    gpioExport(yellowLED);
    gpioSetDirection(yellowLED,outputPin);
    gpioExport(greenLED);
    gpioSetDirection(greenLED,outputPin);

    // Flash the red LED 5 times
    for(int i=0; i<5; i++)
    {
        cout << "Setting the red LED on" << endl;
        gpioSetValue(redLED, high);
        usleep(500000);         // on for 200ms
        cout << "Setting the red LED off" << endl;
        gpioSetValue(redLED, low);
        usleep(500000);         // off for 200ms
    }

    // Flash the yellow LED 5 times
    for(int i=0; i<5; i++)
    {
        cout << "Setting the yellow LED on" << endl;
        gpioSetValue(yellowLED, high);
        usleep(500000);         // on for 200ms
        cout << "Setting the yellow LED off" << endl;
        gpioSetValue(yellowLED, low);
        usleep(500000);         // off for 200ms
    }

    // Flash the green LED 5 times
    for(int i=0; i<5; i++)
    {
        cout << "Setting the green LED on" << endl;
        gpioSetValue(greenLED, high);
        usleep(500000);         // on for 200ms
        cout << "Setting the green LED off" << endl;
        gpioSetValue(greenLED, low);
        usleep(500000);         // off for 200ms
    }

    gpioUnexport(redLED);
    gpioUnexport(yellowLED);
    gpioUnexport(greenLED);

    return 0;
}


