#pragma once

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <string>

struct data_container
{
    unsigned char start = 0xff;
    unsigned char code;
    unsigned char length;
    unsigned char data[8];
    unsigned char stop = 0xfe;
};

class USB
{
    int fd;
    char portname[13] = "/dev/ttyUSB0";

    data_container incoming_data;

    data_container to_send_left_line_visible;
    data_container to_send_right_line_visible;
    data_container to_send_horizontal_pos;
    data_container to_send_left_angle;
    data_container to_send_right_angle;

public:
    USB();
    int init(int speed = B115200);
    bool send_one_chunk(data_container &to_send);
    bool read_one_chunk();
};
