#include "sharedmemory.hpp"

SharedMemory::SharedMemory()
{
    key = 55555;
}

bool SharedMemory::init()
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
        std::cout << "Error trying to read memory for first time" << std::endl;
        return 0;
    }

    return 1;
}

bool SharedMemory::get_access()
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
        std::cout << "Error trying to read memory for first time" << std::endl;
        return 0;
    }

    return 1;
}

void SharedMemory::push_data()
{

}

void SharedMemory::pull_data()
{

}

void SharedMemory::close()
{
    system("ipcrm -M 55555");
}
