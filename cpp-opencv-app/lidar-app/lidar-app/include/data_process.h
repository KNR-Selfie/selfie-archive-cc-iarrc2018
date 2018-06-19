#pragma once

#define FRAME_X 1000
#define FRAME_Y 1000

struct LidarReading
{
    long *data;
    std::vector<cv::Point> pos;
    std::vector<double> angle;
};

void polar_to_cartesian(LidarReading &input, int n, int angle_offset, int scale)
{
    cv::Point new_data;
    input.pos.clear();
    input.angle.clear();

    for(int i = 0; i < n; i++)
    {
        input.angle.push_back((i * 0.36 + angle_offset) * (3.14159/180));
        new_data.x = -cos(input.angle[i]) * input.data[i]/scale + FRAME_X/2;
        new_data.y = sin(input.angle[i]) * input.data[i]/scale + FRAME_Y/2;

        input.pos.push_back(new_data);
    }

}

void filter_angle(LidarReading &input, LidarReading &output, int cut_left, int cut_right)
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

void filter_distance(LidarReading &input, LidarReading &output, int closest, int farthest, int scale)
{
    output.angle.clear();
    output.pos.clear();

    int distance;

    for(uint32_t i = 0; i < input.pos.size(); i++)
    {
        distance = scale * sqrt((input.pos[i].x - FRAME_X/2)*(input.pos[i].x - FRAME_X/2) + (input.pos[i].y - FRAME_Y/2)*(input.pos[i].y - FRAME_Y/2));

        if(distance > closest && distance < farthest)
        {
            output.pos.push_back(input.pos[i]);
            output.angle.push_back(input.angle[i]);
        }
    }
}

void filter_single_points(LidarReading &input, int thresh, LidarReading &output)
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

void draw_data(cv::Mat &frame, LidarReading &input)
{
    cv::circle(frame, cv::Point(FRAME_X/2, FRAME_Y/2), 20, cv::Scalar(0, 255, 255), 5, 8, 0);

    for(uint32_t i = 0; i < input.pos.size(); i++)
        cv::circle(frame, input.pos[i], 2, cv::Scalar(255, 255, 255), 2, 8, 0);
}

void draw_boundaries(cv::Mat frame, LidarReading input, int closest, int farthest, int scale)
{
    if(input.pos.size() > 0)
    {
        cv::line(frame, cv::Point(FRAME_X/2, FRAME_Y/2), input.pos[0], cv::Scalar(255,0,0), 3, 8, 0);
        cv::line(frame, cv::Point(FRAME_X/2, FRAME_Y/2), input.pos[input.pos.size()-1], cv::Scalar(0,255,0), 3, 8, 0);
    }

    cv::circle(frame, cv::Point(FRAME_X/2, FRAME_Y/2), closest/scale, cv::Scalar(0, 0, 255), 5, 8, 0);
    cv::circle(frame, cv::Point(FRAME_X/2, FRAME_Y/2), farthest/scale, cv::Scalar(255, 255, 0), 5, 8, 0);
}
