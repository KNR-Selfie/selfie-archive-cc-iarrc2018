#include "uart_class.hpp"

bool UART::set()
{
    fd = open("/dev/ttySAC0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        std::cout << "\033[1;31m Opening Serial port failed\033[0m\n" << std::endl;
        return 0;
    }
    else
    {
        std::cout << "\033[1;32m Opening Serial port succeded\033[0m\n" << std::endl;
    }

    std::cout << "\033[1;33m Setting Serial options\033[0m\n" << std::endl;

    //Get the current options for the port fd
    tcgetattr(fd, &options);

    //Set the baud rates to 115200
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    //Enable parity
    options.c_cflag |= PARENB;
    //Set parity to even
    options.c_cflag &= ~PARODD;

    //2 stop bits
    options.c_cflag |= CSTOPB;

    //Mask the character size bits and set 8 data bits
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

   //Set the new options for the port...
    tcsetattr(fd, TCSANOW, &options);

    //Clear UART buffer
    tcflush(fd, TCIOFLUSH);

    std::cout << "\033[1;32m Serial options set \033[0m\n" << std::endl;
    return 1;
}

void UART::send_data()
{
    for(int i = 0; i < 14; i++)
    {
        //std::cout << (int)unia_danych.data_to_send[i] << std::endl;
        check = write(fd, &unia_danych.data_to_send[i], 1);
    }
}

