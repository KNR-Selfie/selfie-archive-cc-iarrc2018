#pragma once

#include <iostream>
#include <termios.h>
#include <pthread.h>
#include <stdint.h>
#include <fcntl.h>
#include <cstdlib>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <time.h>

class UART
{
    struct sigaction saio;
    struct termios options;

    int fd = 0;
    bool check = false;

public:
    struct data_frame
    {
        unsigned char sync_byte;
        unsigned short data_0 : 11;
        unsigned short data_1 : 11;
        unsigned short data_2 : 11;
        unsigned short data_3 : 11;
        unsigned short data_4 : 11;
        unsigned short data_5 : 11;
        unsigned short data_6 : 11;
        unsigned short data_7 : 11;
        unsigned char flags;
        unsigned char end_byte;
    } __attribute__((packed));

    union frame
    {
        data_frame dane;
        char data_to_send[14];
    };

    frame unia_danych;

    bool set();
    void send_data();
};

