#include <termios.h>
#include <pthread.h>
#include <stdint.h>
#include <fcntl.h>
#include <cstdlib>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/signal.h>

struct sigaction saio;
struct termios options;

void send_UART_data();