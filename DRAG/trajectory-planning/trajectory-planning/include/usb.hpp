#pragma once

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <string>
#include <vector>


struct data_container
{
    const uint8_t start = 0xFF;
    const uint8_t code = 0xAA;
    uint8_t length = 16;
    const uint8_t stop = 0xFE;
    uint8_t data[16];
};

class USB_STM
{
    char portname[13] = "/dev/ttyACM0";
    unsigned char to_send_packed[22];
    int fd; //file descriptor
public:
    USB_STM();
    int init(int speed = B115200);
    void uint32_to_char_tab(uint32_t input, unsigned char *output);
    void char_tab_to_uint32(unsigned char input[], uint32_t *output);
    void send_buf(data_container &to_send);
    void read_buf(int buf_size,float& velocity, uint16_t &tf_mini,uint8_t &taranis_3_pos,uint8_t &taranis_reset_gear,uint8_t& stm_reset,uint8_t &lights);
    void data_pack(uint32_t velo,uint32_t ang, std::vector<uint32_t>flags,data_container *container);
};


