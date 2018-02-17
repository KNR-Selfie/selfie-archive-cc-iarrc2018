#include "line_class.hpp"
#define HEIGHT 380
#define WIDTH 640
#define BORDER 140
#define MULTIPLICATION 1

void dec_to_bin(char liczba)
{
  int i=31;
  bool ok=false;
  while(i--)
  {
    //warunek, który pozwoli ominąć początkowe zera
    if(liczba>>i&1&!ok) 
      ok=true;
 
    if(ok)
      std::cout<<((liczba>>i)&1);
 
  }

	std::cout << std::endl;
}

void LineDetector::applyBlur(cv::Mat &input, cv::Mat &output)
{
    cv::GaussianBlur(input, output, cv::Size(3,3), 0, 0);
}

void LineDetector::applyBirdEye(cv::Mat &input, cv::Mat &output)
{
	resize(input, input, cv::Size(640, 380));

	double focalLength, dist, alpha; 
	int alpha_ = 20, f_ = 700, dist_ = 700;

	alpha =((double)alpha_ -90) * CV_PI/180;
	focalLength = (double)f_;
	dist = (double)dist_;

	cv::Size image_size = input.size();
	double w = (double)image_size.width, h = (double)image_size.height;

	// Projecion matrix 2D -> 3D
	cv::Mat A1 = (cv::Mat_<float>(4, 3)<< 
		1, 0, -w/2,
		0, 1, -h/2,
		0, 0, 0,
		0, 0, 1 );
	
	// Rotation matrices Rx, Ry, Rz

	cv::Mat RX = (cv::Mat_<float>(4, 4) << 
		1, 0, 0, 0,
		0, cos(alpha), -sin(alpha), 0,
		0, sin(alpha), cos(alpha), 0,
		0, 0, 0, 1 );
/*
		Mat RY = (Mat_<float>(4, 4) << 
			cos(beta), 0, -sin(beta), 0,
			0, 1, 0, 0,
			sin(beta), 0, cos(beta), 0,
			0, 0, 0, 1	);

		Mat RZ = (Mat_<float>(4, 4) << 
			cos(gamma), -sin(gamma), 0, 0,
			sin(gamma), cos(gamma), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1	);
*/

		// R - rotation matrix
		cv::Mat R = RX; //* RY * RZ;

		// T - translation matrix
		cv::Mat T = (cv::Mat_<float>(4, 4) << 
			1, 0, 0, 0,  
			0, 1, 0, 0,  
			0, 0, 1, dist,  
			0, 0, 0, 1); 
		
		// K - intrinsic matrix 
		cv::Mat K = (cv::Mat_<float>(3, 4) << 
			focalLength, 0, w/2, 0,
			0, focalLength, h/2, 0,
			0, 0, 1, 0
			); 

		cv::Mat transformationMat = K * (T * (R * A1));

		cv::warpPerspective(input, output, transformationMat, image_size, cv::INTER_CUBIC | cv::WARP_INVERSE_MAP);
}

void LineDetector::edgeDetect(cv::Mat &input, cv::Mat &output_thresh, cv::Mat &output_edges, int &threshold_value, int morph_size, int morph_elem)
{
  cv::Mat kernel_v;
  cv::Mat kernel_h;

  cv::Mat out_v;
  cv::Mat out_h;

	cv::Mat thresh;

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

  cv::threshold(input, thresh, threshold_value, 255, cv::THRESH_BINARY);

//13.02.2018
//================================================
int operation = 3;


cv::Mat element = getStructuringElement( morph_elem, cv::Size( 2*morph_size + 1, 2*morph_size+1 ), cv::Point( morph_size, morph_size ) );

cv::morphologyEx(thresh, output_thresh, operation, element );

//==================================================
  cv::filter2D(output_thresh, out_v, -1, kernel_v, anchor, 0, cv::BORDER_DEFAULT);
  cv::filter2D(output_thresh, out_h, -1, kernel_h, anchor, 0, cv::BORDER_DEFAULT);

	output_edges = out_v;

  //cv::bitwise_or(out_v, out_h, output_edges);
  //cv::add(out_v, out_h, output_edges);
}

void LineDetector::applyMask(cv::Mat &input, cv::Mat &mask, cv::Mat &output)
{
    cv::bitwise_and(input, mask, output);
}

void LineDetector::detectLines(cv::Mat &input, int &P_min_votes, int &P_max_gap, int &P_min_len)
{
    HoughLinesP(input, lines, 1, CV_PI/180, P_min_votes, P_min_len, P_max_gap);  
	//Bylo: CV_PI/180*10
}

/*
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
*/

void LineDetector::draw_data(cv::Mat &output, cv::Point points[])
{
	//Clear frame
	output = cv::Mat::zeros(360, 640, CV_8UC3);
	
	std::cout << "X: " << last_bottom_middle_point.coordinates.x << std::endl;
	
	int y1 = 230;
	int y2 = 270; 
		
	int x1 = (y1 - (last_bottom_middle_point.coordinates.y - (last_bottom_middle_point.slope * last_bottom_middle_point.coordinates.x))) / last_bottom_middle_point.slope;
	int x2 = (y2 - (last_bottom_middle_point.coordinates.y - (last_bottom_middle_point.slope * last_bottom_middle_point.coordinates.x))) / last_bottom_middle_point.slope;

	//Draw dynamic mask	
	cv::fillConvexPoly(output, points, 6, cv::Scalar(100, 100, 100));

	//Detected middle point
	line(output,
         cv::Point(x1,230),
         cv::Point(x2,270),
         cv::Scalar(255, 255, 0),
         3,
         8);
	//Left line middle
	line(output,
         cv::Point(new_pos_left,230),
         cv::Point(new_pos_left,270),
         cv::Scalar(255, 0, 255),
         3,
         8);
	//Right line middle
	line(output,
         cv::Point(new_pos_right,230),
         cv::Point(new_pos_right,270),
         cv::Scalar(0, 255, 255),
         3,
         8);

	//Left points
    for( size_t i = 0; i < BL_points.size(); i++ )
    {
		circle( output, BL_points[i].coordinates, 2, cv::Scalar(0, 0, 255), 3, 8);
    }

	//Right points
	for( size_t i = 0; i < BR_points.size(); i++ )
    {
		circle( output, BR_points[i].coordinates, 2, cv::Scalar(0, 255, 0), 3, 8);
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

void LineDetector::sort_lines(bool l_change)
{	
    int parking_points = 0;
    int cross_points = 0;

    cv::Point C;
    Punkt SPoint;
	
	TL_points.clear();
	TR_points.clear();
	BL_points.clear();
	BR_points.clear();

	int Ax = last_bottom_middle_point.coordinates.x;
	int Ay = last_bottom_middle_point.coordinates.y; 

	int By = 0;
	int Bx = (By - (Ay - (last_bottom_middle_point.slope * Ax))) / last_bottom_middle_point.slope;	 	

    for(int i = 0; i < all_points.size(); i++)
    {
		//prosty podzia na prawo i lewo:///////////////////////////////////
		////////////////////////////////////////////////////////////////////
		/*SPoint.coordinates.x = all_points[i].coordinates.x;
		SPoint.coordinates.y = all_points[i].coordinates.y;
        SPoint.slope = all_points[i].slope;

		if (all_points[i].coordinates.x < last_bottom_middle_point.coordinates.x)
			BL_points.push_back(SPoint);

		if (all_points[i].coordinates.x > last_bottom_middle_point.coordinates.x)
			BR_points.push_back(SPoint); */
		///////////////////////////////////////////////////////////////////

        //int v = sqrt(((all_points[i].coordinates.x)^2) + ((all_points[i].coordinates.y)^2));
        //int w = sqrt(((all_points[i+1].coordinates.x)^2) + ((all_points[i+1].coordinates.y)^2));
        //int scalar = (all_points[i].coordinates.x * all_points[i+1].coordinates.x) + (all_points[i].coordinates.y * 		  all_points[i+1].coordinates.y);
 
        C.x = all_points[i].coordinates.x;
        C.y = all_points[i].coordinates.y;

    	int alfa = (Ax - Bx)*(C.y - By)-(Ay - By)*(C.x-Bx);

       // float beta = scalar / (v * w);

        SPoint.coordinates = C;
        SPoint.slope = all_points[i].slope;

if(true) // !(abs(SPoint.slope) < 0.15) || l_change
{
        if(alfa > 0 && C.y < BORDER){
            TL_points.push_back(SPoint);
        }

        else if(alfa > 0 && C.y > BORDER){
            BL_points.push_back(SPoint);	
        }

        else if(alfa < 0 && C.y < BORDER){
            TR_points.push_back(SPoint);
        }

        else if(alfa < 0 && C.y > BORDER)
		{
			//float angle_1 = (atan(all_points[i].slope)/CV_PI) * 180;
			//float angle_2 = (atan(new_slope_right)/CV_PI) * 180;

			//if(abs(angle_1 - angle_2) < 50)
           		BR_points.push_back(SPoint);
        }

        else if(alfa == 0){
            horizontal_points.push_back(SPoint);
        }
}		
		float tolerance = 0.2;
        float parking_angle = 0.71;
        float cross_angle = 0;
		
		
        //if((beta == parking_angle - tolerance) || (beta == parking_angle + tolerance))
       // {
           // parking_points++;
        //}

        //if((beta == cross_angle - tolerance) || (beta == cross_angle + tolerance))
        //{
            //cross_points++;
        //}
    }

	if(TL_points.size() > 3)
		TL_sector = true;
	else
		TL_sector = false;

	if(TR_points.size() > 3)
		TR_sector = true;
	else
		TR_sector = false;

	if(BL_points.size() > 3)
		BL_sector = true;
	else
		BL_sector = false;

	if(BR_points.size() > 3)
		BR_sector = true;
	else
		BR_sector = false;

	Horizontal_lines = false;

	std::cout << "TL: " << TL_sector << "  ";
	std::cout << "TR: " << TR_sector << "  ";
	std::cout << "BL: " << BL_sector << "  ";
	std::cout << "BR: " << BR_sector << std::endl;
	std::cout << "HR: " << Horizontal_lines << std::endl;

	//std::cout << "near:" << near.coordinates.x << "::" << near.coordinates.y << std::endl;
	
	//last_top_middle_point.slope = average_slope(BR_points);
	//last_top_middle_point.coordinates = cv::Point(average_pos(BR_points), 250);	

	//std::cout << "1 slope:" << last_top_middle_point.slope << std::endl;
	//std::cout << "1 coords:" << last_top_middle_point.coordinates << std::endl;

	//last_bottom_middle_point.slope = average_slope(BL_points);
	//last_bottom_middle_point.coordinates = cv::Point(average_pos(BL_points), 250);	

	//std::cout << "2 slope:" << last_bottom_middle_point.slope  << std::endl;
	//std::cout << "2 coords:" << last_bottom_middle_point.coordinates << std::endl;

	//std::cout << "Bottom left:" << BL_points.size() << std::endl;
	//std::cout << "Bottom right:" << BR_points.size() << std::endl;	
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
        //std::cout << "No points!" << std::endl;
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

void LineDetector::save_for_next_step()
{
	//calculate left line angle in degrees
	//float left_angle;
	
	//calculate right line angle in degrees
	//float right_angle;

	//calculate average slope
	float average_middle_slope;
	float average_middle_angle; 

//13.02.2018
//===========================================================================
if((!BL_sector) && (!BR_sector))
{
	new_pos_left = 100;
	new_pos_right = 539;
	
	new_slope_left = 2;
	left_ang_st = (atan(new_slope_left)/CV_PI) * 180;
	if(left_ang_st < 0)
		left_ang_st += 180;

	new_slope_right = -2;
	right_ang_st = (atan(new_slope_right)/CV_PI) * 180;	
	if(right_ang_st < 0)
		right_ang_st += 180;
}//===========================================================================
else
{
	//save all data
	if(BL_sector)
		new_pos_left = average_pos(BL_points);
	
	if(BR_sector)
		new_pos_right = average_pos(BR_points);
	
	if(BL_sector)
	{
		new_slope_left = average_slope(BL_points);
		left_ang_st = (atan(new_slope_left)/CV_PI) * 180;
		
		if(left_ang_st < 0)
			left_ang_st += 180;
	}
	
	if(BR_sector)
	{
		new_slope_right = average_slope(BR_points);
		right_ang_st = (atan(new_slope_right)/CV_PI) * 180;	

		if(right_ang_st < 0)
			right_ang_st += 180;
	}

	//Uciekla z lewej
	if(!BL_sector && BR_sector)
	{
		new_pos_left = new_pos_right - width;
	}
	else
	{
		//Uciekla z prawej
		if(BL_sector && !BR_sector)
		{
			new_pos_right = new_pos_left + width;
		}
	}
}
	new_middle = ((new_pos_right - new_pos_left) / 2) + new_pos_left;

	average_middle_angle = (left_ang_st+right_ang_st) / 2;
	
	//if(!BL_sector || !BR_sector)
	average_middle_slope = tan((average_middle_angle / 180) * CV_PI);

	last_bottom_middle_point.coordinates = cv::Point(new_middle, 300);
	last_bottom_middle_point.slope = average_middle_slope;

	std::cout << "Left st:  " << left_ang_st << std::endl;
	std::cout << "Right st: " << right_ang_st << std::endl;

	//std::cout << "Left Slope:  " << new_slope_left << std::endl;
	//std::cout << "Right Slope: " << new_slope_right << std::endl;

	std::cout << "Middle pos:   " << new_middle << std::endl;
	std::cout << "Middle angle: " << average_middle_angle << std::endl;
	//std::cout << "M slope: " << average_middle_slope << std::endl;

	if(BL_sector && BR_sector)
	{
		width = new_pos_right - new_pos_left;
		
		if(width < 380)
            width =	380;
		else if(width > 600)
			width = 600;
	}
	
	std::cout << "Width: " << width << std::endl;


	/*if(BL_sector = false || BR_sector = false)
	{
		last_bottom_middle = last_bottom_middle + (width / 2);
	}*/
}

void LineDetector::send_data_to_main(int &detected_middle_pos_near, int &left_lane_angle_st, int &right_lane_angle_st, char &flags_to_UART)
{
	detected_middle_pos_near = new_middle + 680;
	left_lane_angle_st = left_ang_st;
	right_lane_angle_st = right_ang_st;

flags_to_UART &= ~(1<<0);
flags_to_UART &= ~(1<<1);

	if(BL_sector)
		flags_to_UART |= (1<<7);
	else
		flags_to_UART &= ~(1<<7);
		//flags_to_UART &= 0b01111111;

	if(BR_sector)
		flags_to_UART |= (1<<6);
	else
		flags_to_UART &= ~(1<<6);
		//flags_to_UART &= 0b10111111;

	if(Parking_line)
		flags_to_UART |= (1<<5);
	else
		flags_to_UART &= ~(1<<5);
		//flags_to_UART &= 0b11011111;

	if(Cross_line)
		flags_to_UART |= (1<<4);
	else
		flags_to_UART &= ~(1<<4);
		//flags_to_UART &= 0b11101111;


	std::cout << flags_to_UART << std::endl;

	std::cout << "Lewa linia:" << BL_sector << std::endl;
	std::cout << "Prawa linia:" << BR_sector << std::endl;
	std::cout << "Parking:" << Parking_line << std::endl;
	std::cout << "Skrzyzowanie:"<< Cross_line << std::endl;

	std::cout << "Current lane: " << current_lane << std::endl;
}

void LineDetector::restart_lane_detection(int &UART_offset, bool &check_for_offset, int &mask_y, int &mask_width, int &mask_y_1, int &mask_y_2, int &mask_x_l, int &mask_x_r)
{
	last_top_middle_point.coordinates.x = 320;
	last_bottom_middle_point.coordinates.x = 320;
	
	last_top_middle_point.slope = 999;
	last_bottom_middle_point.slope = 999;
	
	new_slope_left = 999; 
	new_slope_right = 999; 
	new_pos_left = 70; 
	new_pos_right = 570; 
	new_middle = 320;
	width = 500;

	left_ang_st = 90;
	right_ang_st = 90;

	current_lane = 1;
	UART_offset = 0;
	check_for_offset = false;

	mask_y = 200;
	mask_width = 195;
	mask_y_1 = 300;
	mask_y_2 = 340;
	mask_x_l = 0;
	mask_x_r = 639;
}

void LineDetector::cancel_offset(int &UART_offset, bool &check_for_offset) 
{

	if(current_lane)
	{
		if(first_step)
		{
			if(!BL_sector)
			{
				first_step = false;
				second_step = true;
			}
		}

		if(second_step)
		{
			if(BR_sector && new_pos_right > 420)
			{
				first_step = false;
				second_step = false;
				check_for_offset = false;
				UART_offset = 0;
			}
		}
	}
	else
	{
		if(first_step)
		{
			if(!BR_sector)
			{
				first_step = false;
				second_step = true;
			}
		}

		if(second_step)
		{
			if(BL_sector && new_pos_left < 220)
			{
				first_step = false;
				second_step = false;
				check_for_offset = false;
				UART_offset = 0;
			}
		}
	}

/*
	if(current_lane)
	{
		if(first_step)
		{
			if(emergency_counter > 15)
			{
				first_step = false;
				second_step = false;
				check_for_offset = false;
				UART_offset = 0;
			}
			else
			{
				emergency_counter++;			
			}
			
			if(!BL_sector)
			{
				if(offset_counter_1 >= 2)
				{
					first_step = false;
					second_step = true;
					offset_counter_2 = 0;
				}
				else
				{
					offset_counter_1++;
				}			
			}
			else
			{
				offset_counter_1--;
			}
		}

		if(second_step)
		{
			if(BL_sector)
			{
				if(offset_counter_2 >= 4)
				{
					first_step = false;
					second_step = false;
					check_for_offset = false;
					UART_offset = 0;
				}
				else
				{
					offset_counter_2++;
				}			
			}
			else
			{
				offset_counter_2--;
			}
		}
	}
	else
	{
		if(first_step)
		{
			if(emergency_counter > 15)
			{
				first_step = false;
				second_step = false;
				check_for_offset = false;
				UART_offset = 0;
			}
			else
			{
				emergency_counter++;			
			}

			if(!BR_sector)
			{
				if(offset_counter_1 >= 2)
				{
					first_step = false;
					second_step = true;
					offset_counter_2 = 0;
				}
				else
				{
					offset_counter_1++;
				}			
			}
			else
			{
				offset_counter_1 = 0;
			}
		}

		if(second_step)
		{
			if(BR_sector)
			{
				if(offset_counter_2 >= 4)
				{
					first_step = false;
					second_step = false;
					check_for_offset = false;
					UART_offset = 0;
				}
				else
				{
					offset_counter_2++;
				}			
			}
			else
			{
				offset_counter_2 = 0;
			}
		}
	}
*/
}


//
void LineDetector::change_lane(int &UART_offset, bool &check_for_offset)
{
	if(current_lane)
	{
		current_lane = 0;
		check_for_offset = true;
		first_step = true;
		UART_offset = -600;
		offset_counter_1 = 0;
		offset_counter_2 = 0;
		emergency_counter = 0;
	}
	else
	{
		current_lane = 1;
		check_for_offset = true;
		first_step = true;
		UART_offset = 600;
		offset_counter_1 = 0;
		offset_counter_2 = 0;
		emergency_counter = 0;
	}
}


void LineDetector::horizontal_line(cv::Mat frame)
{
    int number_of_pixels = 20;
    int x = new_pos_left + 70;
    int y = 220;
    int width = (new_pos_right - new_pos_left) - 140;
    int height = 120;
    cv::Scalar white_pix;

    if(BL_sector == true && BR_sector == true && width > 0)
    {
        bool left_side = false;
		std::cout << "Lewa_linia:" << new_pos_left << std::endl;		

        if(new_pos_left > 45)
        {
              int number_of_left_pixels = 3;
              int left_x = new_pos_left - 40;
              int left_y = 150;
              int left_width = 30;
              int left_height = 190;
              cv::Scalar left_white_pix;

              cv::Mat left_roi;
              cv::Mat masked_left_roi;
              cv::Rect left_roi_rect(left_x, left_y, left_width, left_height);
              left_roi = frame(left_roi_rect);

              cv::Mat left_mask = cv::Mat::zeros(cv::Size(left_roi.cols, left_roi.rows), CV_8UC1);
              cv::Point left_points [4] =
              {
                  cv::Point(0, 0),
                  cv::Point(0, left_roi.rows),
                  cv::Point(left_roi.cols - 10, left_roi.rows),
                  cv::Point(left_roi.cols - 5, 0)
              };

              cv::fillConvexPoly(left_mask, left_points, 4, cv::Scalar(255, 0, 0));
              cv::bitwise_and(left_roi, left_mask, masked_left_roi);

              left_white_pix = cv::mean(masked_left_roi);
              std::cout << "White left pixels:" << left_white_pix[0] << std::endl;

              if(left_white_pix[0] >= number_of_left_pixels)
              {
                  left_side = true;
              }

              imshow("left roi", masked_left_roi);
        }

        cv::Mat roi;
        cv::Mat masked_roi;
        cv::Rect roi_rect(x, y, width, height);
        roi = frame(roi_rect);

        cv::Mat mask = cv::Mat::zeros(cv::Size(roi.cols, roi.rows), CV_8UC1);
        cv::Point points [4] =
        {
            cv::Point(60, 0),
            cv::Point(0, roi.rows),
            cv::Point(roi.cols, roi.rows),
            cv::Point(roi.cols - 50, 0)
        };

        cv::fillConvexPoly(mask, points, 4, cv::Scalar(255, 0, 0));
        cv::bitwise_and(roi, mask, masked_roi);

        white_pix = cv::mean(masked_roi);
        std::cout << "White pixels:" << white_pix[0] << std::endl;

        if(white_pix[0] >= number_of_pixels)
        {
            if(left_side == false)
             {
                 Cross_line = true;
                 std::cout << "=============CROSS=============" << std::endl;
                 std::cout << "==========CROSS LINE===========" << std::endl;

             }
            else
            {
                Cross_line = false;
            }
            if(left_side == true)
            {
                Parking_line = true;
                std::cout << "=============PARKING=============" << std::endl;
                std::cout << "==========PARKING LINE===========" << std::endl;
            }
            else
            {
                Parking_line = false;
            }
        }
        else
        {
            Cross_line = false;
            Parking_line = false;
        }

        imshow("masked roi", masked_roi);
    }
}



void LineDetector::dynamic_mask (int &left_x, int &right_x, int &mask_y, int &mask_width)
{
    int y = mask_y;
    int mask_middle_x;
	float slope = last_bottom_middle_point.slope;
	int middle_x = last_bottom_middle_point.coordinates.x;
	int middle_y = last_bottom_middle_point.coordinates.y;

	mask_middle_x = ((y - (middle_y - slope * middle_x)) / slope) * MULTIPLICATION;
	if (mask_middle_x < 190) 
	{
		left_x = 1;
		right_x = mask_middle_x + mask_width;
	}
	else if (mask_middle_x > 480) 
	{
		right_x = 639;
		left_x = mask_middle_x - mask_width;
	}
	else
	{
		left_x = mask_middle_x - mask_width;
		right_x = mask_middle_x + mask_width; 
	}
}

void LineDetector::display_last_middle()
{
	std::cout << "LAST MIDDLE: " << last_bottom_middle_point.coordinates.x << std::endl;
}

/*void LineDetector::quick_sort(vector<Punkt> points)
{

} */

