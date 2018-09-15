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
        std::cout << "Error getting memmory access at key: " << key
                  << ", and size of: " << mem_size << "bytes" << std::endl;
        return 0;
    }

    shared_variable = (uint32_t*) shmat(mem_id, NULL, 0);

    if(shared_variable == (uint32_t*) -1)
    {
        std::cout << "Error trying to read from shared memory for the first time" << std::endl;
        return 0;
    }

    return 1;
}

void SharedMemory::push_point_data(std::vector<cv::Point> points)
{
    uint32_t j = 0;
    std::vector<uint32_t> tmp;

    tmp.push_back(0);

    // Pack data
    if(points.size() > 0)
    {
        for(uint32_t i = 0; i < points.size(); i++)
        {
            tmp.push_back(points[i].x);
            tmp.push_back(points[i].y);
            j+=2;
        }

        tmp[0] = j;
     }

    // Copy data to shm
    memcpy(shared_variable, &tmp[0], 4*(j+1));
}

void SharedMemory::pull_points_data(cv::Mat &out, cv::Scalar color)
{
    cv::Point tmp;
    std::vector<cv::Point> vector_points;

    std::cout << "Length: " << shared_variable[0] << std::endl;

    if(shared_variable[0])
    {
        for(uint32_t i = 1; i <= shared_variable[0]; i+=2)
        {
            std::cout << "X: " << shared_variable[i] << ", Y: " << shared_variable[i+1] << std::endl;

            tmp.x = shared_variable[i];
            tmp.y = shared_variable[i+1];
            vector_points.push_back(tmp);
        }
    }

    for(uint32_t i = 0; i < vector_points.size(); i++)
    {
        cv::circle(out, vector_points[i], 2, color, CV_FILLED, cv::LINE_AA);
    }
}

void SharedMemory::push_additional_data(cv::Point lidar_pos, int scale, cv::Point gap_pos_left, cv::Point gap_pos_right)
{
    uint32_t tmp[8];

    tmp[0] = 3;
    tmp[1] = lidar_pos.x;
    tmp[2] = lidar_pos.y;
    tmp[3] = scale;
    tmp[4] = gap_pos_left.x;
    tmp[5] = gap_pos_left.y;
    tmp[6] = gap_pos_right.x;
    tmp[7] = gap_pos_right.y;

    // Copy data to shm
    memcpy(shared_variable, &tmp[0], 32);
}

void SharedMemory::pull_additional_data()
{
    std::cout << "Length:        " << shared_variable[0] << std::endl;
    std::cout << "Lidar pos:     " << "[" << shared_variable[1] << "," << shared_variable[2] << "]" << std::endl;
    std::cout << "Scale:         " << shared_variable[3] << std::endl;
    std::cout << "Gap pos left:  " << "[" << shared_variable[4] << "," << shared_variable[5] << "]" << std::endl;
    std::cout << "Gap pos right: " << "[" << shared_variable[6] << "," << shared_variable[7] << "]" << std::endl;
}

void SharedMemory::close()
{
    std::string command = "ipcrm -M ";
    command += std::to_string(key);
    system(command.c_str());
}
