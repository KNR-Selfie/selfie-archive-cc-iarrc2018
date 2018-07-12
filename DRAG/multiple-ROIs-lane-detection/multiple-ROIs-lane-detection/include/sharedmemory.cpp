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

void SharedMemory::push_lane_data(std::vector<cv::Point> left_points, std::vector<cv::Point> right_points, std::vector<cv::Point> vector_cones)
{
    std::vector<uint32_t> tmp;

    int head = 0;
    int j = 1;

    // Left line
    tmp.push_back(0);

    if(left_points.size() > 0)
    {
        for(uint32_t i = 0; i < left_points.size(); i++)
        {
            tmp.push_back(left_points[i].x);
            tmp.push_back(left_points[i].y);

            j+=2;
        }

        tmp[head] = j-1;
        tmp.push_back(5000);
        head += j+1;
    }
    else
    {
        tmp.push_back(5000);
        head += 2;
    }

    // Right line
    j = 1;

    tmp.push_back(0);

    if(right_points.size() > 0)
    {
        for(uint32_t i = 0; i < right_points.size(); i++)
        {
            tmp.push_back(right_points[i].x);
            tmp.push_back(right_points[i].y);

            j+=2;
        }

        tmp[head] = j-1;
        tmp.push_back(5000);
        head += j+1;
    }
    else
    {
        tmp.push_back(5000);
        head += 2;
    }

    // Cones
    j = 1;

    tmp.push_back(0);

    if(vector_cones.size() > 0)
    {
        for(uint32_t i = 0; i < vector_cones.size(); i++)
        {
            tmp.push_back(vector_cones[i].x);
            tmp.push_back(vector_cones[i].y);

            j+= 2;
        }

        tmp[head] = j-1;
        head += j;
    }
    else
    {
        head ++;
    }

    // Copy data to shm
    if(head != 0)
        memcpy(shared_variable, &tmp[0], 4*head);
}

void SharedMemory::pull_lane_data(cv::Mat &test)
{
    std::vector<cv::Point> vector_left;
    std::vector<cv::Point> vector_right;
    std::vector<cv::Point> vector_cones;

    uint32_t head = 0;
    uint32_t i = 1;
    cv::Point tmp;

    std::cout << "=== SHM LANE ===" << std::endl;
    std::cout << "Length left: " << shared_variable[head] << std::endl;

    if(shared_variable[head])
    {
        for(; i < shared_variable[head]; i+=2)
        {
            std::cout << "X: " << shared_variable[i] << ", Y: " << shared_variable[i+1] << std::endl;

            tmp.x = shared_variable[i];
            tmp.y = shared_variable[i+1];
            vector_left.push_back(tmp);
        }
    }

    std::cout << "Check: 5000 = " << shared_variable[i] << std::endl;

    i++;
    head = i;
    i++;
    std::cout << "Length right: " << shared_variable[head] << std::endl;

    if(shared_variable[head])
    {
        for(; i < shared_variable[head] + head; i+=2)
        {
            std::cout << "X: " << shared_variable[i] << ", Y: " << shared_variable[i+1] << std::endl;

            tmp.x = shared_variable[i];
            tmp.y = shared_variable[i+1];
            vector_right.push_back(tmp);
        }
    }

    std::cout << "Check: 5000 = " << shared_variable[i] << std::endl;

    i++;
    head = i;
    i++;
    std::cout << "Length cones: " << shared_variable[head] << std::endl;

    if(shared_variable[head])
    {
        for(; i < shared_variable[head] + head; i+=2)
        {
            std::cout << "X: " << shared_variable[i] << ", Y: " << shared_variable[i+1] << std::endl;

            tmp.x = shared_variable[i];
            tmp.y = shared_variable[i+1];
            vector_cones.push_back(tmp);
        }
    }

    i++;
    head = i;
    i++;

    for(uint32_t i = 0; i < vector_left.size(); i++)
    {
        cv::circle(test, vector_left[i], 2, cv::Scalar(0, 0, 255), CV_FILLED, cv::LINE_AA);
    }

    for(uint32_t i = 0; i < vector_right.size(); i++)
    {
        cv::circle(test, vector_right[i], 2, cv::Scalar(0, 255, 0), CV_FILLED, cv::LINE_AA);
    }

    for(uint32_t i = 0; i < vector_cones.size(); i++)
    {
        cv::circle(test, vector_cones[i], 2, cv::Scalar(255, 0, 0), CV_FILLED, cv::LINE_AA);
    }
}

void SharedMemory::push_scene_data(uint32_t red_light_visible, uint32_t green_light_visible)
{
    uint32_t tmp[7];

    tmp[0] = 5;
    tmp[1] = red_light_visible;
    tmp[2] = green_light_visible;
    tmp[3] = shared_variable[3];
    tmp[4] = shared_variable[4];
    tmp[5] = shared_variable[5];
    tmp[6] = 1;

    // Copy data to shm
    memcpy(shared_variable, &tmp[0], 28);
}

void SharedMemory::pull_scene_data(uint32_t &reset_stop, uint32_t &reset_lane, uint32_t &light_3_pos, uint32_t &ping)
{
    reset_stop = shared_variable[3];
    reset_lane= shared_variable[4];
    light_3_pos= shared_variable[5];
    //ping = shared_variable[6];
#ifdef VERBOSE_MODE
    std::cout << std::endl << "SCENE SHM: " << std::endl;
    std::cout << "Length:    " << shared_variable[0] << std::endl;
    std::cout << "Red vis:   " << shared_variable[1] << std::endl;
    std::cout << "Green vis: " << shared_variable[2] << std::endl;
    std::cout << "Reset stop:  " << shared_variable[3] << std::endl;
    std::cout << "Reset lane:  " << shared_variable[4] << std::endl;
    std::cout << "Ping:  " << shared_variable[5] << std::endl;
#endif
}

void SharedMemory::close()
{
    std::string command = "ipcrm -M ";
    command += std::to_string(key);
    system(command.c_str());
}
