#include "watchdog_class.hpp"

bool Watchdog::init()
{
    mem_id = shmget(key, MEMSIZE, IPC_CREAT | 0666);
    if(mem_id < 0)
    {
        std::cout << "Error getting memmory access at given key" << std::endl;
        return 0;
    }

    shared_variable = (char*) shmat(mem_id, NULL, 0);

    if(shared_variable == (char*) -1)
    {
        std::cout << "Error at first memmory read" << std::endl;
        return 0;
    }

    return 1;
}

bool Watchdog::get_access()
{
    mem_id = shmget(key, MEMSIZE, 0666);
    if(mem_id < 0)
    {
        std::cout << "Error getting memmory access at given key" << std::endl;
        return 0;
    }

    shared_variable = (char*) shmat(mem_id, NULL, 0);

    if(shared_variable == (char*) -1)
    {
        std::cout << "Error at first memmory read" << std::endl;
        return 0;
    }

    return 1;
}

void Watchdog::push_flag(bool flag)
{
    std::string text;

    if(flag)
    {
        memcpy(shared_variable, "1", 2);
    }
    else
    {
        memcpy(shared_variable, "0", 2);
    }

    end_of_variable = shared_variable;
    end_of_variable += 1;
    *end_of_variable = 0;
}

bool Watchdog::pull_flag()
{
    if(*shared_variable == '1')
        return 1;
    else
        return 0;
}

