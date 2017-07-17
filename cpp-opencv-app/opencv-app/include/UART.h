#include <iostream>
#include <bitset>
#include <termios.h>
#include <pthread.h>
#include <stdint.h>
#include <fcntl.h>
#include <cstdlib>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/signal.h>

using namespace std;

struct sigaction saio;
struct termios options;

struct data_frame
{
	unsigned char sync_byte = 0xff;
	unsigned short data_0 : 11;
	unsigned short data_1 : 11;
	unsigned short data_2 : 11;
	unsigned short data_3 : 11;
	unsigned short data_4 : 11;
	unsigned short data_5 : 11;
	unsigned short data_6 : 11;
	unsigned short data_7 : 11;
	unsigned char flags = 0;
	unsigned char end_byte = 0xff;
} __attribute__((packed));

union frame
{
	data_frame dane;
	unsigned char data_to_send[14];
};

frame unia_danych;

int fd = 0;
bool check = false;

bool UART_set();
void send_UART_data();