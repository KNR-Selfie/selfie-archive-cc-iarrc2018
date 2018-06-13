#include "sharedmemory.hpp"

SharedMemory::SharedMemory(key_t k, int m_size)
{
    key = k;
    mem_size = m_size;
}

bool SharedMemory::init()
{
    mem_id = shmget(key, mem_size, IPC_CREAT | 0666);
    if(mem_id < 0)
    {
        std::cout << "Error getting memmory access at given key" << std::endl;
        return 0;
    }

    shared_variable = (uint32_t*) shmat(mem_id, NULL, 0);

    if(shared_variable == (uint32_t*) -1)
    {
        std::cout << "Error trying to read memory for first time" << std::endl;
        return 0;
    }

    return 1;
}

bool SharedMemory::get_access()
{
    mem_id = shmget(key, mem_size, 0666);
    if(mem_id < 0)
    {
        std::cout << "Error getting memmory access at given key" << std::endl;
        return 0;
    }

    shared_variable = (uint32_t*) shmat(mem_id, NULL, 0);

    if(shared_variable == (uint32_t*) -1)
    {
        std::cout << "Error trying to read memory for first time" << std::endl;
        return 0;
    }

    return 1;
}

void SharedMemory::push_data(std::vector<std::vector<cv::Point>> vector)
{
    uint32_t tmp[25000];
    int j = 1;

    // Data vector to int[]
    for(uint32_t i = 0; i < vector.size(); i++)
    {
        for(uint32_t k = 0; k < vector[i].size(); k++)
        {
            tmp[j] = vector[i][k].x;
            tmp[j+1] = vector[i][k].y;

            j+= 2;
        }
    }
    tmp[0] = j;
    std::cout << "Wykryto: " << j << std::endl;

    // Copy data to shm
    memcpy(shared_variable, &tmp[0], 4*j);
}


void SharedMemory::pull_data(std::vector<cv::Point> &point_vector)
{   uint32_t lenght = shared_variable[0];

        for(int i =0;i<lenght;i++){
            point_vector.push_back(cv::Point(shared_variable[2*i+1],shared_variable[2*i+2]));
        }



}

void SharedMemory::close()
{
    std::string command = "ipcrm -M ";
    command += std::to_string(key);
    system(command.c_str());
}
uint32_t SharedMemory::get_lenght()
{
    return shared_variable[0];
}
///////////////////////////////////////////////////////////////////////////////






