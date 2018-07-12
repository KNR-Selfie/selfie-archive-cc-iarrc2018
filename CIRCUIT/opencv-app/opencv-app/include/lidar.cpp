#include "lidar.hpp"

Lidar::Lidar()
{

}

int Lidar::init()
{
    // Try to start communication
    if(urg_open(&urg, URG_SERIAL, connect_device, connect_baudrate) < 0)
    {
        std::cout << "Can not open serial communication with sensor" << std::endl;
        std:: cout << "Closing app" << std:: endl;
        return -1;
    }
    else
    {
        std::cout << "Connected" << std::endl;
    }

    // Check sensor
    if(!(urg_is_stable(&urg)))
    {
        std::cout << "Sensor is unstable and can not do measurement!" << std::endl;
        std::cout << "Closing app" << std::endl;
        return -2;
    }

    return 1;
}

void Lidar::allocate_memory()
{
    // Allocate memmory for data
    data_size = urg_max_data_size(&urg);
    data = (long *)malloc(data_size * sizeof(long));
}

void Lidar::calculate_offset()
{
    // Calculate angle offset for OpenCV drawing
    urg_start_measurement(&urg, URG_DISTANCE, 1, 0);
    n = urg_get_distance(&urg, data, NULL);
    angle_offset = 150 - ((data_size - n) * 0.36) / 4;
    //urg_stop_measurement(&urg);
}

void Lidar::start_measurement()
{
    urg_start_measurement(&urg, URG_DISTANCE, 1, 0);
}

void Lidar::read_new_data()
{
    if(urg_start_measurement(&urg, URG_DISTANCE, 1, 0) < 0)
            {
                std::cout << "Can not start measurement" << std::endl;
                std:: cout << "Closing app" << std:: endl;
                //return -3;
    }
    n = urg_get_distance(&urg, data, NULL);
}

void Lidar::polar_to_cartesian()
{
    cv::Point new_data;
    double angle;
    data_points.clear();

    for(int i = 0; i < n; i++)
    {
        angle = (i * 0.36 + angle_offset) * (3.14159/180);
        new_data.x = -cos(angle) * data[i] + FRAME_X/2;
        new_data.y = sin(angle) * data[i] + FRAME_Y/2;

        data_points.push_back(new_data);
    }

}

void Lidar::draw_data(cv::Mat &data_frame)
{
    cv::circle(data_frame, cv::Point(FRAME_X/2, FRAME_Y/2), 20, cv::Scalar(255, 255, 255), 5, 8, 0);

    cv::line(data_frame, cv::Point(FRAME_X/2, FRAME_Y/2), data_points[0], cv::Scalar(255,0,0), 3, 8, 0);
    cv::line(data_frame, cv::Point(FRAME_X/2, FRAME_Y/2), data_points[n-1], cv::Scalar(0,255,0), 3, 8, 0);

    for(int i = 0; i < n; i++)
        cv::circle(data_frame, data_points[i], 2, cv::Scalar(0, 0, 255), 2, 8, 0);
}

void Lidar::stop_measurement()
{
    urg_stop_measurement(&urg);
}
