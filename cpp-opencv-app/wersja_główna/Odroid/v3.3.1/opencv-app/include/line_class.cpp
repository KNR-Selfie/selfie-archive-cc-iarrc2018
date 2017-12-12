#include "line_class.hpp"

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

  cv::threshold(input, output_thresh, threshold_value, 255, cv::THRESH_BINARY);

  cv::filter2D(output_thresh, out_v, -1, kernel_v, anchor, 0, cv::BORDER_DEFAULT);
  cv::filter2D(output_thresh, out_h, -1, kernel_h, anchor, 0, cv::BORDER_DEFAULT);

  //cv::bitwise_or(out_v, out_h, output_edges);
  cv::add(out_v, out_h, output_edges);
}

void LineDetector::applyMask(cv::Mat &input, cv::Mat &mask, cv::Mat &output)
{
    cv::bitwise_and(input, mask, output);
}

void LineDetector::detectLines(cv::Mat &input)
{
    HoughLinesP(input, lines, 1, CV_PI/180, 8, 10, 5);  //Bylo: CV_PI/180*10

    //calculate_all_slopes();
    //average_slope(all_points);

}

void LineDetector::drawLines(cv::Mat &output)
{
    output = cv::Mat::zeros(380, 640, CV_8UC1);

    for( size_t i = 0; i < lines.size(); i++ )
    {
        line(output,
             cv::Point(lines[i][0], lines[i][1]),
             cv::Point(lines[i][2], lines[i][3]),
             cv::Scalar(255, 0, 0),
             3,
             8);
    }
}

void LineDetector::calculate_all_slopes()
{
    Punkt tmp;

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

void LineDetector::sort_lines()
{
	//Funkcja do podzialu std::vector<Punkt> all_points na 5 x std::vector<Punkt>
}

float LineDetector::average_slope(std::vector<Punkt> &punkty)
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

        //std::cout << "Slope: " << average_slope_near << std::endl;
        //std::cout << "Dodatnich: " << count_dodatnie << std::endl;
        //std::cout << "Ujemnych: " << count_ujemne << std::endl;
    }
    else
    {
        std::cout << "No points!" << std::endl;
    }
}

int LineDetector::average_pos(std::vector<Punkt> &punkty)
{
	int average_pos = 0;

	for(size_t i = 0; i < punkty.size(); i++)
    {
		average_pos += punkty[i].coordinates.x;
	}

	average_pos = average_pos / punkty.size();

	return average_pos;
}

void LineDetector::save_for_next_step(float &new_left_slope, float &new_right_slope, int &new_center_pos)
{
	//calculate left line angle in degrees
	float left_angle;


	//calculate right line angle in degrees
	float right_angle;


	//calculate average slope
	float average_slope; 
	
	//save all data
	last_bottom_middle_point.coordinates = cv::Point(new_center_pos,250);
	last_bottom_middle_point.slope = average_slope;

	srodek = new_center_pos;
	lewy_kat = left_angle;
	prawy_kat = right_angle;
}

