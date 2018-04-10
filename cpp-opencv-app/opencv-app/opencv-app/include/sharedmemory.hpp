#pragma once

#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MEMSIZE 64

class SharedMemory
{
    int mem_id;
    key_t  key;
    char *shared_variable;
    char *end_of_variable;

public:
    SharedMemory();
    bool init();
    bool get_access();
    void push_data();
    void pull_data();
    void close();
};
