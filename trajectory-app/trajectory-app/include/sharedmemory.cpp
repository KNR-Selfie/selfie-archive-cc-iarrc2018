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

void SharedMemory::push_data(uint8_t taranis_3_pos, uint8_t taranis_reset_gear,uint8_t stm_reset)
{

    uint32_t tmp[7];

    tmp[0] = 6;
    tmp[1] = shared_variable[1];
    tmp[2] = shared_variable[2];
    tmp[3] = taranis_reset_gear;
    tmp[4] = stm_reset;
    tmp[5] = taranis_3_pos;
    tmp[6] = 1;

    // Copy data to shm
    memcpy(shared_variable, &tmp[0], 28);
}

void SharedMemory::pull_lidar_data(std::vector<cv::Point>&l_vector)
{
    uint32_t l_length = shared_variable[0];
    if(l_length>0)
    {
        for(int i=0;i<l_length;i++)
        {
            l_vector.push_back(cv::Point(shared_variable[2*i+1],shared_variable[2*i+2]));
        }
    }
}

void SharedMemory::pull_add_data(std::vector<uint32_t>&data)
{
    if(mem_id>0)
    {
        if(shared_variable[0] == 3)
        {
            for(int i=0;i<7;i++)
            {
                data[i]=shared_variable[i+1];
            }
        }
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

void SharedMemory::push_signal(uint32_t signal)
{
    uint32_t tmp[1] = {signal};

    // Copy data to shm
    memcpy(shared_variable, tmp, 4);
}
bool SharedMemory::pull_signal()
{
    if(shared_variable[0])
        return true;
    return false;
}
