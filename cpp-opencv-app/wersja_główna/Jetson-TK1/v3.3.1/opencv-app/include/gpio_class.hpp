#pragma once
 
#define DIR "/sys/class/gpio"
#define POLL_TIMEOUT (3000)
#define MAX_BUF 64

typedef unsigned int pinDirection;
typedef unsigned int pinValue;
typedef unsigned int jetsonGPIO;

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

enum jetsonGPIONumber
{
    gpio57  =  57,    // J3A1 - Pin 50
    gpio160 = 160,	  // J3A2 - Pin 40
    gpio161 = 161,    // J3A2 - Pin 43
    gpio162 = 162,    // J3A2 - Pin 46
    gpio163 = 163,    // J3A2 - Pin 49
    gpio164 = 164,    // J3A2 - Pin 52
    gpio165 = 165,    // J3A2 - Pin 55
    gpio166 = 166     // J3A2 - Pin 58
}  ;

class GPIO
{
public:
    jetsonGPIO pushButton = gpio163;
    jetsonGPIO redLED = gpio164;
    jetsonGPIO yellowLED = gpio165;
    jetsonGPIO greenLED = gpio166;

    int Export (jetsonGPIO gpio);
    int Unexport (jetsonGPIO gpio);
    int SetDirection (jetsonGPIO gpio, pinDirection out_flag);
    int SetValue (jetsonGPIO gpio, pinValue value);
    int GetValue (jetsonGPIO gpio, unsigned int *value);
    int SetEdge (jetsonGPIO gpio, char *edge);
    int Open (jetsonGPIO gpio);
    int Close (int fileDescriptor);
};
