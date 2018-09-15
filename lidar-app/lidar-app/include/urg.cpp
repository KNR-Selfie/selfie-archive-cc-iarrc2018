#include "urg.hpp"

// Serial communication parameters
const char connect_device[] = "/dev/ttyACM0";
const long connect_baudrate = 115200;

URG::URG()
{

}

int URG::init()
{
    // Try to start communication
    if(urg_open(&urg, URG_SERIAL, connect_device, connect_baudrate) < 0)
    {
        std::cout << "Can not open serial communication with sensor on port " << connect_device << std::endl;
        return -1;
    }
    else
    {
        std::cout << "Connected to lidar on port " << connect_device << std::endl;
    }

    // Check sensor
    if(!(urg_is_stable(&urg)))
    {
        std::cout << "Sensor is unstable and can not do measurement!" << std::endl;
        return -2;
    }
    else
    {
        std::cout << "Sensor is stable" << std::endl;
    }

    // Allocate memmory for data
    data_size = urg_max_data_size(&urg);
    raw_data.data = (long *)malloc(data_size * sizeof(long));

    // Calculate angle offset for OpenCV drawing
    urg_start_measurement(&urg, URG_DISTANCE, 1, 0);
    n = urg_get_distance(&urg, raw_data.data, NULL);
    angle_offset = 150 - ((data_size - n) * 0.36) / 4;

    return 1;
}

int URG::read()
{
    // Start sensor
    if(urg_start_measurement(&urg, URG_DISTANCE, 1, 0) < 0)
    {
        std::cout << "Error getting new data from sensor" << std::endl;
        return -1;
    }

    // Get new data from sensor
    n = urg_get_distance(&urg, raw_data.data, NULL);
    return n;
}

void URG::polar_to_cartesian()
{
    cv::Point new_data;
    raw_data.pos.clear();
    raw_data.angle.clear();

    for(int i = 0; i < n; i++)
    {
        raw_data.angle.push_back((i * 0.36 + angle_offset) * (3.14159/180));
        new_data.x = -cos(raw_data.angle[i]) * raw_data.data[i]/scale + WIDTH/2;
        new_data.y = sin(raw_data.angle[i]) * raw_data.data[i]/scale + HEIGHT/2;

        raw_data.pos.push_back(new_data);
    }
}

void URG::filter_angle(LidarReading &input, LidarReading &output)
{
    output.angle.clear();
    output.pos.clear();

    double right = cut_left*(3.14159/180);
    double left = cut_right*(3.14159/180);

    for(uint32_t i = 0; i < input.angle.size(); i++)
    {
        if(input.angle[i] > right && input.angle[i] < left)
        {
            output.pos.push_back(input.pos[i]);
            output.angle.push_back(input.angle[i]);
        }
    }
}

void URG::filter_distance(LidarReading &input, LidarReading &output)
{
    output.angle.clear();
    output.pos.clear();

    int distance;

    for(uint32_t i = 0; i < input.pos.size(); i++)
    {
        distance = scale * sqrt((input.pos[i].x - WIDTH/2)*(input.pos[i].x - WIDTH/2) + (input.pos[i].y - HEIGHT/2)*(input.pos[i].y - HEIGHT/2));

        if(distance > closest && distance < farthest)
        {
            output.pos.push_back(input.pos[i]);
            output.angle.push_back(input.angle[i]);
        }
    }
}

void URG::filter_X(LidarReading &input, LidarReading &output)
{
    output.angle.clear();
    output.pos.clear();

    for(uint32_t i = 0; i < input.pos.size(); i++)
    {
        if(input.pos[i].x > x_left && input.pos[i].x < x_right)
        {
            output.pos.push_back(input.pos[i]);
            output.angle.push_back(input.angle[i]);
        }
    }
}

void URG::filter_Y(LidarReading &input, LidarReading &output)
{
    output.angle.clear();
    output.pos.clear();

    for(uint32_t i = 0; i < input.pos.size(); i++)
    {
        if(input.pos[i].y < y_down && input.pos[i].y > y_up)
        {
            output.pos.push_back(input.pos[i]);
            output.angle.push_back(input.angle[i]);
        }
    }
}

void URG::filter_single_points(LidarReading &input, LidarReading &output)
{
    cv::Point tmp;
    bool last = false;

    output.pos.clear();

    for(uint32_t i = 0; i < input.pos.size() - 1; i++)
    {
        tmp = input.pos[i] - input.pos[i+1];
        if(cv::sqrt(tmp.x*tmp.x + tmp.y*tmp.y) < thresh)
        {
            last = true;
            output.pos.push_back(input.pos[i]);
            output.angle.push_back(input.angle[i]);
        }
        else
        {
            if(last)
                output.pos.push_back(input.pos[i]);

            last = false;
        }
    }
}

void URG::draw_data_raw(cv::Mat &frame)
{
    cv::circle(frame, cv::Point(WIDTH/2, HEIGHT/2), 20, cv::Scalar(0, 255, 255), 5, 8, 0);

    for(uint32_t i = 0; i < raw_data.pos.size(); i++)
        cv::circle(frame, raw_data.pos[i], 2, cv::Scalar(255, 255, 255), 2, 8, 0);
}

void URG::draw_data_filtered(cv::Mat &frame, LidarReading &filtered)
{
    cv::circle(frame, cv::Point(WIDTH/2, HEIGHT/2), 20, cv::Scalar(0, 255, 255), 5, 8, 0);

    for(uint32_t i = 0; i < filtered.pos.size(); i++)
        cv::circle(frame, filtered.pos[i], 2, cv::Scalar(255, 255, 255), 2, 8, 0);
}

void URG::draw_boundaries(cv::Mat &frame, LidarReading &input)
{
    if(input.pos.size() > 0)
    {
        cv::line(frame, cv::Point(WIDTH/2, HEIGHT/2), input.pos[0], cv::Scalar(255,0,0), 3, 8, 0);
        cv::line(frame, cv::Point(WIDTH/2, HEIGHT/2), input.pos[input.pos.size()-1], cv::Scalar(0,255,0), 3, 8, 0);
    }

    cv::line(frame, cv::Point(x_left,0), cv::Point(x_left, HEIGHT-1), cv::Scalar(0,0,255), 2, 8, 0);
    cv::line(frame, cv::Point(x_right,0), cv::Point(x_right, HEIGHT-1), cv::Scalar(255,255,0), 2, 8, 0);

    cv::line(frame, cv::Point(0,y_up), cv::Point(WIDTH-1, y_up), cv::Scalar(255,0,255), 2, 8, 0);
    cv::line(frame, cv::Point(0,y_down), cv::Point(WIDTH-1, y_down), cv::Scalar(0,255,255), 2, 8, 0);

    cv::circle(frame, cv::Point(WIDTH/2, HEIGHT/2), closest/scale, cv::Scalar(128, 128, 128), 5, 8, 0);
    cv::circle(frame, cv::Point(WIDTH/2, HEIGHT/2), farthest/scale, cv::Scalar(128, 0, 128), 5, 8, 0);
}


void URG::close()
{
    // Stop sensor
    urg_stop_measurement(&urg);

    // Disconnect from the sensor
    urg_close(&urg);

    std::cout << "Disconnected from lidar" << std::endl;
}
