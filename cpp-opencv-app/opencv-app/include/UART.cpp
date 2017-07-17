#include "UART.h"

bool UART_set()
{
    fd = open("/dev/ttyTHS1", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
		cout << "\033[1;31m Opening Serial port failed\033[0m\n" << endl;
		return 0;
    }
    else
    {
		cout << "\033[1;32m Opening Serial port succeded\033[0m\n" << endl;
    }

    cout << "\033[1;33m Setting Serial options\033[0m\n" << endl;

    //Setting UART options
    //Get the current options for the port
    tcgetattr(fd, &options);

    //Set the baud rates to 115200
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    //Enable the receiver and set local mode
    options.c_cflag |= (CLOCAL | CREAD);
	
    //Enable parity
    options.c_cflag |= PARENB;
	//Set parity to even
	options.c_cflag &= ~PARODD;
	
    //2 stop bits
    options.c_cflag |= CSTOPB;
	
    //Mask the character size bits and set 8 data bits
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    //Disable hardware flow control
    options.c_cflag &= ~CRTSCTS;

    //Choosing raw input
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    //
    options.c_cflag |= (CLOCAL | CREAD);

    //Disabling software flow control
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    //
    options.c_oflag &= ~OPOST;

   //Set the new options for the port...
    tcsetattr(fd, TCSANOW, &options);

    //Clear UART buffer
    tcflush(fd, TCIOFLUSH);	
	
    cout << "\033[1;32m Serial options set \033[0m\n" << endl;
	return 1;
}

void send_UART_data()
{
	for(int i = 0; i < 14; i++)
    {
		check = write(fd, &unia_danych.data_to_send[i], 14);
    }
}