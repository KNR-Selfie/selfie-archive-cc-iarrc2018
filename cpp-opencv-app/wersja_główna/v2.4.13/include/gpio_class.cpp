/*
 * Date: 20.11.2017
 * Author: Mateusz Grudzien
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include "gpio_class.hpp"

int GPIO::gpioExport(int gpio)
{
    int fileDescriptor, length;
    char commandBuffer[MAX_BUF];

    fileDescriptor = open(DIR "/export", O_WRONLY);
    if (fileDescriptor < 0)
    {
        char errorBuffer[128];
        snprintf(errorBuffer,sizeof(errorBuffer), "gpioExport unable to open gpio%d",gpio);
        perror(errorBuffer);
        return fileDescriptor;
    }

    length = snprintf(commandBuffer, sizeof(commandBuffer), "%d", gpio);

    if (write(fileDescriptor, commandBuffer, length) != length)
    {
        perror("gpioExport");
        return fileDescriptor;
    }

    close(fileDescriptor);
    return 0;
}

int GPIO::gpioUnexport(int gpio)
{
    int fileDescriptor, length;
    char commandBuffer[MAX_BUF];

    fileDescriptor = open(DIR "/unexport", O_WRONLY);

    if (fileDescriptor < 0)
    {
        char errorBuffer[128];
        snprintf(errorBuffer,sizeof(errorBuffer), "gpioUnexport unable to open gpio%d",gpio);
        perror(errorBuffer);
        return fileDescriptor;
    }

    length = snprintf(commandBuffer, sizeof(commandBuffer), "%d", gpio);

    if (write(fileDescriptor, commandBuffer, length) != length)
    {
        perror("gpioUnexport");
        return fileDescriptor;
    }

    close(fileDescriptor);
    return 0;
}

int GPIO::gpioSetDirection(int gpio, unsigned int out_flag)
{
    int fileDescriptor;
    char commandBuffer[MAX_BUF];

    snprintf(commandBuffer, sizeof(commandBuffer), DIR  "/gpio%d/direction", gpio);

    fileDescriptor = open(commandBuffer, O_WRONLY);

    if (fileDescriptor < 0)
    {
        char errorBuffer[128];
        snprintf(errorBuffer,sizeof(errorBuffer), "gpioSetDirection unable to open gpio%d",gpio);

        perror(errorBuffer);
        return fileDescriptor;
    }

    if (out_flag)
    {
        if (write(fileDescriptor, "out", 4) != 4)
        {
            perror("gpioSetDirection");
            return fileDescriptor;
        }
    }
    else
    {
        if (write(fileDescriptor, "in", 3) != 3)
        {
            perror("gpioSetDirection");
            return fileDescriptor;
        }
    }

    close(fileDescriptor);
    return 0;
}

int GPIO::gpioSetValue(int gpio, unsigned int value)
{
    int fileDescriptor;
    char commandBuffer[MAX_BUF];

    snprintf(commandBuffer, sizeof(commandBuffer), DIR "/gpio%d/value", gpio);

    fileDescriptor = open(commandBuffer, O_WRONLY);
    if (fileDescriptor < 0)
    {
        char errorBuffer[128];
        snprintf(errorBuffer,sizeof(errorBuffer), "gpioSetValue unable to open gpio%d",gpio);
        perror(errorBuffer);
        return fileDescriptor;
    }

    if (value)
    {
        if (write(fileDescriptor, "1", 2) != 2)
        {
            perror("gpioSetValue");
            return fileDescriptor;
        }
    }
    else
    {
        if (write(fileDescriptor, "0", 2) != 2)
        {
            perror("gpioSetValue");
            return fileDescriptor;
        }
    }

    close(fileDescriptor);
    return 0;
}

int GPIO::gpioGetValue(int gpio, unsigned int *value)
{
    int fileDescriptor;
    char commandBuffer[MAX_BUF];
    char ch;

    snprintf(commandBuffer, sizeof(commandBuffer), DIR "/gpio%d/value", gpio);

    fileDescriptor = open(commandBuffer, O_RDONLY);
    if (fileDescriptor < 0)
    {
        char errorBuffer[128];
        snprintf(errorBuffer,sizeof(errorBuffer), "gpioGetValue unable to open gpio%d",gpio);
        perror(errorBuffer);
        return fileDescriptor;
    }

    if (read(fileDescriptor, &ch, 1) != 1)
    {
        perror("gpioGetValue");
        return fileDescriptor;
     }

    if (ch != '0') {
        *value = 1;
    }
    else
    {
        *value = 0;
    }

    close(fileDescriptor);
    return 0;
}

int GPIO::gpioSetEdge(int gpio, char *edge )
{
    int fileDescriptor;
    char commandBuffer[MAX_BUF];

    snprintf(commandBuffer, sizeof(commandBuffer), DIR "/gpio%d/edge", gpio);

    fileDescriptor = open(commandBuffer, O_WRONLY);
    if (fileDescriptor < 0)
    {
        char errorBuffer[128];
        snprintf(errorBuffer,sizeof(errorBuffer), "gpioSetEdge unable to open gpio%d",gpio);

        perror(errorBuffer);
        return fileDescriptor;
    }

    if (write(fileDescriptor, edge, strlen(edge) + 1) != ((int)(strlen(edge) + 1)))
    {

        perror("gpioSetEdge");
        return fileDescriptor;
    }
    close(fileDescriptor);
    return 0;
}

int GPIO::gpioOpen(int gpio)
{
    int fileDescriptor;
    char commandBuffer[MAX_BUF];

    snprintf(commandBuffer, sizeof(commandBuffer), DIR "/gpio%d/value", gpio);

    fileDescriptor = open(commandBuffer, O_RDONLY | O_NONBLOCK );
    if (fileDescriptor < 0)
    {
        char errorBuffer[128];
        snprintf(errorBuffer,sizeof(errorBuffer), "gpioOpen unable to open gpio%d",gpio);
        perror(errorBuffer);
    }

    return fileDescriptor;
}

int GPIO::gpioClose(int fileDescriptor)
{
    return close(fileDescriptor);
}
