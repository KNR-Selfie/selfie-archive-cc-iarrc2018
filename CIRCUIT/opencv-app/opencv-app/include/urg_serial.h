#ifndef URG_SERIAL_H
#define URG_SERIAL_H

/*!
  \file
  \brief Serial communications
  \author Satofumi KAMIMURA

  $Id$
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "urg_detect_os.h"

#if defined(URG_WINDOWS_OS)
#include <windows.h>
#else
#include <termios.h>
#endif
#include "urg_ring_buffer.h"


enum {
    RING_BUFFER_SIZE_SHIFT = 7,
    RING_BUFFER_SIZE = 1 << RING_BUFFER_SIZE_SHIFT,

    ERROR_MESSAGE_SIZE = 256,
};


//!  Control information for serial connection
typedef struct
{
#if defined(URG_WINDOWS_OS)
    HANDLE hCom;                //!<  Connection resource
    int current_timeout;        //!<  Timeout configuration value
#else
    int fd;                     //!<  File descriptor
    struct termios sio;         //!<  Connection configuration
#endif

    ring_buffer_t ring;         //!<  Ring buffer structure
    char buffer[RING_BUFFER_SIZE]; //!<  Data buffer
    char has_last_ch;          //!<  Whether the last character was received or not
    char last_ch;              //!<  Last character received
} urg_serial_t;


//!  Opens the connection
extern int serial_open(urg_serial_t *serial, const char *device, long baudrate);


//!  Closes the connection
extern void serial_close(urg_serial_t *serial);


//!  Configures the baudrate
extern int serial_set_baudrate(urg_serial_t *serial, long baudrate);


//!  Sends data over serial connection
extern int serial_write(urg_serial_t *serial, const char *data, int size);


//!  Gets data from serial connection
extern int serial_read(urg_serial_t *serial,
                       char *data, int max_size, int timeout);


//!  Gets data from serial connection until end-of-line
extern int serial_readline(urg_serial_t *serial,
                           char *data, int max_size, int timeout);


//!  Stores the serial error message
extern int serial_error(urg_serial_t *serial,
                        char *error_message, int max_size);

#ifdef __cplusplus
}
#endif

#endif /* !URG_SERIAL_H */
