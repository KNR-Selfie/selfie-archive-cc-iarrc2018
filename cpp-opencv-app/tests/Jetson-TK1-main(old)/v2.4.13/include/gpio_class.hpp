/*
 * Date: 20.11.2017
 * Author: Mateusz Grudzien
*/

#pragma once
 
#define DIR "/sys/class/gpio"
#define POLL_TIMEOUT (3000)
#define MAX_BUF 64

typedef unsigned int pinDirection;
typedef unsigned int pinValue;

enum pinDirections
{
    input  = 0,
    output = 1
};

enum pinValues
{
    low = 0,
    high = 1,
};

class GPIO
{
public:
    int gpioExport (int gpio);
    int gpioUnexport (int gpio);
    int gpioSetDirection (int, pinDirection out_flag);
    int gpioSetValue (int gpio, pinValue value);
    int gpioGetValue (int gpio, unsigned int *value);
    int gpioSetEdge (int gpio, char *edge);
    int gpioOpen (int gpio);
    int gpioClose (int fileDescriptor);
};
