#pragma once

#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MEMSIZE 64

class SharedMemory
{
public:
    SharedMemory();
};
