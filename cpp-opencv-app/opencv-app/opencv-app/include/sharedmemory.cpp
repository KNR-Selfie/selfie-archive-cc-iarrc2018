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

void SharedMemory::push_lane_data(std::vector<std::vector<cv::Point>> vector_yellow, std::vector<std::vector<cv::Point>> vector_white, std::vector<cv::Point> vector_cones)
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
        tmp[head] = j-1;
        tmp[head+j] = 5000;
        head += j+1;
        // std::cout << std::endl << "Detected yellow: " << (j-1)/2 << std::endl;
    }
    else
    {
        std::cout << "Pusto - yellow" << std::endl;
        tmp[head] = 0;
        tmp[head+1] = 5000;
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
        tmp[head] = j-1;
        tmp[head+j] = 5000;
        head += j;
        // std::cout << "Detected white: " << (j-1)/2 << std::endl;
    }
    else
    {
        std::cout << "Pusto - white" << std::endl;
        tmp[head] = 0;
        tmp[head+1] = 5000;
        head += 2;
    }


    //Cones
    j = 1;

    if(vector_cones.size() > 0)
    {
        for(uint32_t i = 0; i < vector_cones.size(); i++)
        {
            tmp[head+j] = vector_cones[i].x;
            tmp[head+j+1] = vector_cones[i].y;

            j+= 2;
        }

        tmp[head] = j-1;
        head += j;
        // std::cout << "Detected cones: " << (j-1)/2 << std::endl << std::endl;
    }
    else
    {
        std::cout << "Pusto - cones" << std::endl;
        tmp[head] = 0;
        head ++;
    }

    // Copy data to shm
    if(head != 0)
        memcpy(shared_variable, &tmp[0], 4*head);
}

void SharedMemory::pull_lane_data(cv::Mat &test)
{
    std::vector<cv::Point> vector;

    // std::cout << "Length: " << shared_variable[0] << std::endl;

    int j = 1;
    cv::Point tmp;

    for(int i = j; i < 1000; i+=2)
    {
        // std::cout << "X: " << shared_variable[i] << ", Y: " << shared_variable[i+1] << std::endl;
        if(shared_variable[i] != 5000)
        {
            tmp.x = shared_variable[i];
            tmp.y = shared_variable[i+1];
            vector.push_back(tmp);
        }
        else
        {
            // std::cout << "Koniec!" << std::endl;
            break;
        }
    }

    // std::cout << std::endl;
    // std::cout << "Length: " << shared_variable[0] << std::endl;

    for(int i = j; i < 1000; i+=2)
    {
        // std::cout << "X: " << shared_variable[i] << ", Y: " << shared_variable[i+1] << std::endl;
        if(shared_variable[i] != 5000)
        {
            tmp.x = shared_variable[i];
            tmp.y = shared_variable[i+1];
            vector.push_back(tmp);
        }
        else
        {
            // std::cout << "Koniec!" << std::endl;
            break;
        }
    }

    // std::cout << std::endl;
    // std::cout << "Length: " << shared_variable[0] << std::endl;

    for(int i = j; i < 1000; i+=2)
    {
        // std::cout << "X: " << shared_variable[i] << ", Y: " << shared_variable[i+1] << std::endl;
        if(shared_variable[i] != 5000)
        {
            tmp.x = shared_variable[i];
            tmp.y = shared_variable[i+1];
            vector.push_back(tmp);
        }
        else
        {
            // std::cout << "Koniec!" << std::endl;
            break;
        }
    }
    // std::cout << std::endl;

    for(uint32_t i = 0; i < vector.size(); i++)
    {
        cv::circle(test, vector[i], 2, cv::Scalar(0, 255, 255), CV_FILLED, cv::LINE_AA);
    }


}

void SharedMemory::push_scene_data(bool reset_stm, bool red_light_visible, bool green_light_visible, bool stop_line_detected, uint32_t stop_line_distance)
{
    uint32_t tmp[8];

    tmp[0] = 5;
    tmp[1] = reset_stm;
    tmp[2] = red_light_visible;
    tmp[3] = green_light_visible;
    tmp[4] = stop_line_detected;
    tmp[5] = stop_line_distance;

    // Copy data to shm
    memcpy(shared_variable, &tmp[0], 24);
}

void SharedMemory::pull_scene_data()
{
    std::cout << std::endl << "SCENE SHM: " << std::endl;
    std::cout << "Length:    " << shared_variable[0] << std::endl;
    std::cout << "Reset STM: " << shared_variable[1] << std::endl;
    std::cout << "Red vis:   " << shared_variable[2] << std::endl;
    std::cout << "Green vis: " << shared_variable[3] << std::endl;
    std::cout << "Stop vis:  " << shared_variable[4] << std::endl;
    std::cout << "Stop dist: " << shared_variable[5] << std::endl;
}

void SharedMemory::close()
{
    std::string command = "ipcrm -M ";
    command += std::to_string(key);
    system(command.c_str());
}
