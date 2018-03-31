#ifndef USB_HPP
#define USB_HPP

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <string>

class USB
{
    int fd;
    char portname[13];

public:
    USB();
    int init(int speed = B115200)
;
};

#endif // USB_HPP
