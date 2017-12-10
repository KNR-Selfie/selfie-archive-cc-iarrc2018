#include <string>
//#include <vector>
#include "opencv2/opencv.hpp"

#include "line_class.hpp"

//private:
/*
void LineDetector::calculateCenterPoint(std::vector<cv::Vec4i> &lines)
{
    float x_pos_near = 0;
    float y_pos_near = 0;

    float x_pos_far = 0;
    float y_pos_far = 0;

    int near_count = 0;
    int far_count = 0;

    for( size_t i = 0; i < lines.size(); i++ )
    {
        if(lines[i][1] > Row4Bottom)
        {
            x_pos_near += lines[i][0];
            //y_pos_near += lines[i][1];

            near_count++;
        }
        else
        {
            x_pos_far += lines[i][0];
            //y_pos_far += lines[i][1];

            far_count++;
        }

        if(lines[i][3] > Row4Bottom)
        {
            x_pos_near += lines[i][2];
            //y_pos_near += lines[i][3];

            near_count++;
        }
        else
        {
            x_pos_far += lines[i][2];
            //y_pos_far += lines[i][3];

            far_count++;
        }

        if(lines[i][1] < Row4Top)
        {
            x_pos_near += lines[i][0];
            //y_pos_near += lines[i][1];

            near_count++;
        }
        else
        {
            x_pos_far += lines[i][0];
            //y_pos_far += lines[i][1];

            far_count++;
        }

        if(lines[i][3] < Row4Top)
        {
            x_pos_near += lines[i][2];
            //y_pos_near += lines[i][3];

            near_count++;
        }
        else
        {
            x_pos_far += lines[i][2];
            //y_pos_far += lines[i][3];

            far_count++;
        }
    }

    x_pos_near = x_pos_near / near_count;
    //y_pos_near = y_pos_near / near_count;

    x_pos_far = x_pos_far / far_count;
    //y_pos_far = y_pos_far / far_count;

    CenterPointNear = cv::Point(x_pos_near, 280);
    CenterPointFar = cv::Point(x_pos_far, 140);
}

void LineDetector::calculate_bisector(int &left_lane_angle_st, int &right_lane_angle_st)
{
    double bisector_angle_rad;

    if(CenterPointNear.x == CenterPointFar.x)
    {
        left_lane_angle_st = 90;
        right_lane_angle_st = 90;
    }
    else
    {
        double a = CenterPointNear.y - CenterPointFar.y;
        double b = abs(CenterPointFar.x - CenterPointNear.x);

        if(CenterPointNear.x < CenterPointFar.x)
        {
            bisector_angle_rad = 3.1416 - atan(a / b);
        }
        else
        {
            bisector_angle_rad = atan(a / b);
        }
    }

    left_lane_angle_st = bisector_angle_rad * 57.295;
    right_lane_angle_st = left_lane_angle_st;

    std::cout << "Kat: " << right_lane_angle_st << std::endl;
}
*/

void LineDetector::calculate_slopes(std::vector<cv::Vec4i> &lines)
{
    LineDetector::Punkt tmp;

    all_points.clear();

    for(size_t i = 0; i < lines.size(); i++)
    {
        tmp.slope = float((lines[i][3] - lines[i][1]))/float((lines[i][2] - lines[i][0]));

        if(tmp.slope > 500)
            tmp.slope = 500;

        if(tmp.slope < -500)
            tmp.slope = -500;

        tmp.coordinates = cv::Point(lines[i][0], lines[i][1]);
        all_points.push_back(tmp);

        tmp.coordinates = cv::Point(lines[i][2], lines[i][3]);
        all_points.push_back(tmp);

    }
}

void LineDetector::average_slope(std::vector<Punkt> punkty)
{
    if(punkty.size() > 2)
    {
        int count_ujemne = 0;
        int count_dodatnie = 0;

        float average_slope_near;
        float average_slope_ujemny;
        float average_slope_dodatni;

        for(size_t i = 0; i < punkty.size(); i++)
        {
            if(punkty[i].slope < 0)
            {
                count_ujemne++;
                average_slope_ujemny += punkty[i].slope;
            }
            else
            {
                count_dodatnie++;
                average_slope_dodatni += punkty[i].slope;
            }
        }

        if(count_dodatnie > count_ujemne)
            average_slope_near = average_slope_dodatni/count_dodatnie;
        else
            average_slope_near = average_slope_ujemny/count_ujemne;

        std::cout << "Slope: " << average_slope_near << std::endl;
        std::cout << "Dodatnich: " << count_dodatnie << std::endl;
        std::cout << "Ujemnych: " << count_ujemne << std::endl;
    }
    else
    {
        std::cout << "No points!" << std::endl;
    }
}

//public:

void LineDetector::applyBlur(cv::Mat &input, cv::Mat &output)
{
    cv::GaussianBlur(input, output, cv::Size(3,3), 0, 0);
}

void LineDetector::edgeDetect(cv::Mat &input, cv::Mat &output_thresh, cv::Mat &output_edges, int &threshold_value)
{
  cv::Mat kernel_v;
  cv::Mat kernel_h;

  cv::Mat out_v;
  cv::Mat out_h;

  cv::Point anchor;
  anchor = cv::Point(-1, -1);

  kernel_v = cv::Mat(1, 3, CV_32F);
  kernel_v.at<float>(0, 0) = -1;
  kernel_v.at<float>(0, 1) = 0;
  kernel_v.at<float>(0, 2) = 1;

  kernel_h = cv::Mat(3, 1, CV_32F);
  kernel_h.at<float>(0, 0) = 1;
  kernel_h.at<float>(1, 0) = 0;
  kernel_h.at<float>(2, 0) = -1;

  cv::threshold(input, output_thresh, threshold_value, 255, cv::THRESH_BINARY_INV);

  cv::filter2D(output_thresh, out_v, -1, kernel_v, anchor, 0, cv::BORDER_DEFAULT);
  cv::filter2D(output_thresh, out_h, -1, kernel_h, anchor, 0, cv::BORDER_DEFAULT);

  //cv::bitwise_or(out_v, out_h, output_edges);
  cv::add(out_v, out_h, output_edges);
}

void LineDetector::applyMask(cv::Mat &input, cv::Mat &mask, cv::Mat &output)
{
    cv::bitwise_and(input, mask, output);
}

void LineDetector::detectLines(cv::Mat &input, std::vector<cv::Vec4i> &output_lines)
{
    HoughLinesP(input, output_lines, 1, CV_PI/180*10, 8, 10, 5);

    calculate_slopes(output_lines);
    average_slope(all_points);

}

void LineDetector::drawLines(std::vector<cv::Vec4i> &input_lines, cv::Mat &output)
{
    output = cv::Mat::zeros(380, 640, CV_8UC1);

    for( size_t i = 0; i < input_lines.size(); i++ )
    {
        line(output,
             cv::Point(input_lines[i][0], input_lines[i][1]),
             cv::Point(input_lines[i][2], input_lines[i][3]),
             cv::Scalar(255, 0, 0),
             3,
             8);
    }
}

void LineDetector::writePoints()
{
    //for( size_t i = 0; i < all_points.size(); i++ )
    //{
        //std::cout << "X: " << all_points[i].coordinates.x << ", Y: " << all_points[i].coordinates.y << ", Slope: " <<  all_points[i].slope << std::endl;
    //}
}

/*
void LineDetector::get2points(std::vector<cv::Vec4i> &lines, cv::Mat &frame_lines, std::vector<cv::Point> &lane_corners)
{
    calculateCenterPoint(lines);

    line(frame_lines,
         cv::Point(0, Row4Top),
         cv::Point(639, Row4Top),
         cv::Scalar(255, 0, 0),
         3,
         8);

    line(frame_lines,
         cv::Point(0, Row4Bottom),
         cv::Point(639, Row4Bottom),
         cv::Scalar(255, 0, 0),
         3,
         8);

    cv::circle(frame_lines, CenterPointNear, 5, cv::Scalar(255,0,0), 5, CV_FILLED, 0);
    cv::circle(frame_lines, CenterPointFar, 5, cv::Scalar(255,0,0), 5, CV_FILLED, 0);
}

void LineDetector::calculateDataToSend(std::vector<cv::Point> &lane_corners, int &detected_middle_pos_near, int &detected_middle_pos_far, int &left_lane_angle_st, int &right_lane_angle_st, int8_t &flags_to_UART)
{
    detected_middle_pos_far = CenterPointFar.x;
    detected_middle_pos_near = CenterPointNear.x;

    calculate_bisector(left_lane_angle_st, right_lane_angle_st);
}
*/
