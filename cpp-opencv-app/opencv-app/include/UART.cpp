#include "UART.h"

void UART_set()
{
    //Open port for UART
    fd = open("/dev/ttyTHS1", O_RDWR | O_NOCTTY | O_NDELAY);  //albo /ttyTHS1
    if (fd == -1)
    {
  	cout << "\033[1;31m Opening Serial port failed\033[0m\n" << endl;
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
	
    //No parity
    options.c_cflag &= ~PARENB;
	
    //1 stop bit
    options.c_cflag &= ~CSTOPB;
	
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
}

void send_UART_data()
{
	
}