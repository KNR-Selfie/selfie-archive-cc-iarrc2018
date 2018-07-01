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



void SharedMemory::pull_line_data(std::vector<cv::Point> &y_vector,std::vector<cv::Point> &w_vector,std::vector<cv::Point> &c_vector)
{   uint32_t y_lenght = shared_variable[0];
    uint32_t w_length;
    uint32_t c_length;


    if(y_lenght>0)
    {
        for(int i =0;i<y_lenght/2;i++){

            y_vector.push_back(cv::Point(shared_variable[2*i+1],shared_variable[2*i+2]));
        }
    }
    //check next block begin
    int index = y_lenght+1;


    if(shared_variable[index] == 5000)
    {
        w_length = shared_variable[index+1];
        if(w_length>0)
        {
            int begin = index+1;
            for(int i=0;i<w_length/2;i++)
            {
                w_vector.push_back(cv::Point(shared_variable[begin+2*i+1],shared_variable[begin+2*i+2]));
            }
        }
    }

    index = index + w_length+2;

    if(shared_variable[index] == 5000)
    {   c_length = shared_variable[index+1];
        if(c_length>0)
        {
            int begin = index+1;
            for(int i=0;i<c_length/2;i++)
            {
                c_vector.push_back(cv::Point(shared_variable[begin+2*i+1],shared_variable[begin+2*i+2]));
            }
        }
    }

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
void SharedMemory::pull_usb_data(std::vector<uint32_t>&data)
{
    if(shared_variable[0] == 5)
    {
        for(int i=0;i<5;i++)
        {
            data[i]=shared_variable[i+1];
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
