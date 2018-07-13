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



void SharedMemory::pull_line_data(std::vector<cv::Point> &y_vector,std::vector<cv::Point> &w_vector,std::vector<cv::Point> &c_vector)
{   uint32_t y_lenght = shared_variable[0];
    uint32_t w_length = 0;
    uint32_t c_length = 0;
    int index = 0;


    if(y_lenght>0)
    {
        for(int i =0;i<y_lenght/2;i++){

            y_vector.push_back(cv::Point(shared_variable[2*i+1],shared_variable[2*i+2]));
        }
    }
    //check next block begin
    index = y_lenght+1;


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
    if(mem_id>0)
    {
        if(shared_variable[0] == 6)
        {
            for(int i=0;i<6;i++)
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
