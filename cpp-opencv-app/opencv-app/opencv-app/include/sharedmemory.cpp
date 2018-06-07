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

void SharedMemory::push_data(std::vector<std::vector<cv::Point>> vector_yellow, std::vector<std::vector<cv::Point>> vector_white, std::vector<std::vector<cv::Point>> vector_cones)
{
    uint32_t tmp[25000];
    int j = 1;
    int head = 0;

    // Yellow lines
    if(vector_yellow.size() > 0)
    {
        // Data vector to int[]
        for(uint32_t i = 0; i < vector_yellow.size(); i++)
        {
            for(uint32_t k = 0; k < vector_yellow[i].size(); k++)
            {
                tmp[head+j] = vector_yellow[i][k].x;
                tmp[head+j+1] = vector_yellow[i][k].y;

                j+= 2;
            }
        }
        tmp[head] = j;
        tmp[head+j] = 1000;
        head += j;
        std::cout << std::endl << "Detected yellow: " << j/2 << std::endl;
    }
    else
    {
        tmp[head] = 0;
        tmp[head+1] = 1000;
        head += 2;
    }


    //White lines
    j = 1;

    if(vector_white.size() > 0)
    {
        // Data vector to int[]
        for(uint32_t i = 0; i < vector_white.size(); i++)
        {
            for(uint32_t k = 0; k < vector_white[i].size(); k++)
            {
                tmp[head+j] = vector_white[i][k].x;
                tmp[head+j+1] = vector_white[i][k].y;

                j+= 2;
            }
        }
        tmp[head] = j;
        tmp[head+j] = 1000;
        head += j;
        std::cout << "Detected white: " << j/2 << std::endl;
    }
    else
    {
        tmp[head] = 0;
        tmp[head+1] = 1000;
        head += 2;
    }


    //Cones
    j = 1;

    if(vector_cones.size() > 0)
    {
        // Data vector to int[]
        for(uint32_t i = 0; i < vector_cones.size(); i++)
        {
            for(uint32_t k = 0; k < vector_cones[i].size(); k++)
            {
                tmp[head+j] = vector_cones[i][k].x;
                tmp[head+j+1] = vector_cones[i][k].y;

                j+= 2;
            }
        }
        tmp[head] = j;
        head += j;
        std::cout << "Detected cones: " << j/2 << std::endl << std::endl;
    }
    else
    {
        tmp[head] = 0;
        tmp[head+1] = 1000;
        head +=2;
    }


    // Copy data to shm
    if(head != 0)
        memcpy(shared_variable, &tmp[0], 4*head);
}

void SharedMemory::pull_data()
{
    for(int i = 0; i < 20; i++)
        std::cout << shared_variable[i];

    std::cout << std::endl;
}

void SharedMemory::close()
{
    std::string command = "ipcrm -M ";
    command += std::to_string(key);
    system(command.c_str());
}
