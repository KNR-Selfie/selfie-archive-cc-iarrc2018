#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MEMSIZE 8

class Watchdog
{
    int mem_id;
    key_t  key = 55555;
    char *shared_variable;
    char *end_of_variable;

public:
    bool init();
    bool get_access();
    void push_flag(bool flag);
    bool  pull_flag();
};
