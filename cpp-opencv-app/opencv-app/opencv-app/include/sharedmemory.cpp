#include "sharedmemory.hpp"

SharedMemory::SharedMemory(key_t k)
{
    key = k;
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

void SharedMemory::push_data(std::vector<std::vector<cv::Point>> vector)
{
    uint32_t tmp[128];
    int j = 0;

    // vector to int[]
    for(uint32_t i = 0; i < vector.size(); i++)
    {
        for(uint32_t k = 0; k < vector[i].size(); k++)
        {
            //std::cout << "X: " <<vector[i][k].x << ", Y: " << vector[i][k].y << std::endl;
            tmp[j] = vector[i][k].x;
            tmp[j+1] = vector[i][k].y;

            //std::cout << "X: " << tmp[j] << ", Y: " << tmp[j+1] << std::endl;
            j+= 2;
        }
    }

    unsigned char tmp_char[512];
    j = 0;

    // int[] to char[]
    for(uint32_t i = 0; i < 2*vector.size(); i++)
    {
        tmp_char[j] = (unsigned char) (tmp[i] >> 24 &0xff);
        tmp_char[j + 1] = (unsigned char) (tmp[i] >> 16 &0xff);
        tmp_char[j + 2] = (unsigned char) (tmp[i] >> 8 &0xff);
        tmp_char[j + 3] = (unsigned char) (tmp[i] &0xff);
        j+=4;
    }

    tmp_char[j+1] = '\0';

    //std::string tmp_string(tmp_char);

    //for(uint32_t i = 0; i < 8*vector.size(); i++)
        //memcpy(shared_variable, tmp_string, 8*vector.size());

}

void SharedMemory::pull_data()
{

}

void SharedMemory::close()
{
    system("ipcrm -M 50001");
    system("ipcrm -M 50002");
    system("ipcrm -M 50003");
    system("ipcrm -M 50004");
}

