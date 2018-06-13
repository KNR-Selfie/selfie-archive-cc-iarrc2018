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

void SharedMemory::push_data(std::vector<cv::Point> data)
{
    uint32_t tmp[30000];
    int k = 1;


    for(uint32_t i = 0; i < data.size(); i++)
    {
        tmp[k] = data[i].x;
        tmp[k+1] = data[i].y;
        k+=2;
    }

    // Write data length
    tmp[0] = k;

    // Copy data to shm
    if(k != 0)
        memcpy(shared_variable, &tmp[0], 4*k);
}

void SharedMemory::pull_data(cv::Mat &frame)
{
    std::vector<cv::Point> vector;

    std::cout << "Length: " << shared_variable[0] << std::endl;

    int j = 1;
    cv::Point tmp;

    for(uint32_t i = j; i < shared_variable[0]; i+=2)
    {
        tmp.x = shared_variable[i];
        tmp.y = shared_variable[i+1];
        vector.push_back(tmp);
    }

    for(uint32_t i = 0; i < vector.size(); i++)
    {
        cv::circle(frame, vector[i], 2, cv::Scalar(0, 255, 255), CV_FILLED, cv::LINE_AA);
    }
}

void SharedMemory::close()
{
    std::string command = "ipcrm -M ";
    command += std::to_string(key);
    system(command.c_str());
}
