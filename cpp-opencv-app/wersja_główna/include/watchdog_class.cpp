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

void Watchdog::push_flag(unsigned short flag)
{
    switch(flag)
    {
    case 0:
        memcpy(shared_variable, "0", 2);
        break;

    case 1:
        memcpy(shared_variable, "1", 2);
        break;

    case 2:
        memcpy(shared_variable, "2", 2);
        break;

    default:
        std::cout << "Error wrong argument given!" << std::endl;
        break;
    }

    end_of_variable = shared_variable;
    end_of_variable += 1;
    *end_of_variable = 0;
}

unsigned short Watchdog::pull_flag()
{
    switch(*shared_variable)
    {
    case '0':
        return 0;
        break;

    case '1':
        return 1;
        break;

    case '2':
        return 2;
        break;

    default:
        std::cout << "Error unknown flag in shared memmory!" << std::endl;
        break;
    }

    return 10;
}

void Watchdog::close()
{
    system("ipcrm -M 55555");
}

