/************************************************************************
Project:
Selfie, automonous car
Authot:
Kornelia Lukojc
Descripiton:
Fle include methods definition that can be used with StopLightDetector class
*************************************************************************/

#include "stoplights.h"



#define FRAME_WIDTH 640 //1280
#define FRAME_HEIGHT 480 //720
#define THRESH_LEVEL 35
#define BLUR_SIZE 15

//methode is making roi from input and returning it to output, number tells method with roi, you can test it with test_roi
void StopLightDetector::make_roi(cv::Mat &input, cv::Mat &output, int number) {
	int upper_roi = FRAME_HEIGHT / 5;
	int middle_roi = FRAME_WIDTH / 2;
	int width_roi = FRAME_WIDTH / 3;
	int height_roi = FRAME_WIDTH / 2;
	cv::Rect roi_rectangle1 = cv::Rect(middle_roi - width_roi, upper_roi, width_roi, height_roi);
	cv::Rect roi_rectangle2 = cv::Rect(middle_roi, upper_roi, width_roi, height_roi);
	cv::Rect roi_rectangle3 = cv::Rect(0, upper_roi, width_roi, height_roi);
	cv::Rect roi_rectangle4 = cv::Rect(FRAME_WIDTH - width_roi, upper_roi, width_roi, height_roi);
	cv::Rect roi_rectangle5 = cv::Rect(0, 0, middle_roi, FRAME_HEIGHT);
	cv::Rect roi_rectangle6 = cv::Rect(middle_roi, 0, middle_roi, FRAME_HEIGHT);
	
	switch (number) {
	case 1:
		output = input(roi_rectangle1);
		break;
	case 2:
		output = input(roi_rectangle2);
		break;
	case 3:
		output = input(roi_rectangle3);
		break;
	case 4:
		output = input(roi_rectangle4);
		break;
	case 5:
		output = input(roi_rectangle5);
		break;
	case 6:
		output = input(roi_rectangle6);
		break;
	}
}

//methode that tells us which roi is which
void StopLightDetector::test_roi(cv::Mat &input, cv::Mat &output) {
    start_finding = true;
	int upper_roi = FRAME_HEIGHT / 5;
	int middle_roi = FRAME_WIDTH / 2;
	int width_roi = FRAME_WIDTH / 3;
	int height_roi = FRAME_WIDTH / 2;
	cv::Rect roi_rectangle1 = cv::Rect(middle_roi - width_roi, upper_roi, width_roi, height_roi);
	cv::Rect roi_rectangle2 = cv::Rect(middle_roi, upper_roi, width_roi, height_roi);
	cv::Rect roi_rectangle3 = cv::Rect(0, upper_roi, width_roi, height_roi);
	cv::Rect roi_rectangle4 = cv::Rect(FRAME_WIDTH - width_roi, upper_roi, width_roi, height_roi);
	cv::Rect roi_rectangle5 = cv::Rect(0, 0, middle_roi, FRAME_HEIGHT);
	cv::Rect roi_rectangle6 = cv::Rect(middle_roi, 0, middle_roi, FRAME_HEIGHT);
	input.copyTo(output);
	rectangle(output, roi_rectangle1, cv::Scalar(100, 0, 0), 5, 8, 0);
	putText(output, "1", cv::Point(middle_roi - width_roi, 100), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(100, 0, 0), 9, 8, false);
	rectangle(output, roi_rectangle2, cv::Scalar(0, 100, 0), 5, 18, 0);
	putText(output, "2", cv::Point(middle_roi, 100), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 100, 0), 9, 8, false);
	rectangle(output, roi_rectangle3, cv::Scalar(0, 0, 100), 5, 18, 0);
	putText(output, "3", cv::Point(0, 100), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 0, 100), 9, 8, false);
	rectangle(output, roi_rectangle4, cv::Scalar(100, 100, 0), 5, 18, 0);
	putText(output, "4", cv::Point(FRAME_WIDTH - width_roi, 100), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(100, 100, 0), 9, 8, false);
	rectangle(output, roi_rectangle5, cv::Scalar(100, 0, 100), 5, 18, 0);
	putText(output, "5", cv::Point(0, 200), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(100, 0, 100), 9, 8, false);
	rectangle(output, roi_rectangle6, cv::Scalar(0, 100, 100), 5, 18, 0);
	putText(output, "6", cv::Point(middle_roi, 200), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 100, 100), 9, 8, false);
	
}

//methode returns Mat with value (hsv) from input image
void StopLightDetector::split_bright(cv::Mat &input, cv::Mat &output) {
	cv::Mat hsv(FRAME_WIDTH, FRAME_HEIGHT, CV_8UC4);
	std::vector<cv::Mat> hsv_split;
	cv::cvtColor(input, hsv, CV_RGB2HSV);
	cv::split(hsv, hsv_split);
	hsv_split[2].copyTo(output);
}

//methode returning better image, whole application can be used without it
void StopLightDetector::prepare_image(cv::Mat &input, cv::Mat &output) {
	cv::threshold(input, input, THRESH_LEVEL, 255, cv::THRESH_BINARY);
	cv::blur(input, input, cv::Size(BLUR_SIZE, BLUR_SIZE));
    cv::threshold(input, output, THRESH_LEVEL, 255, cv::THRESH_BINARY);
}

//methode returning to output differences between new_frame and old_frame
void StopLightDetector::give_difference(cv::Mat &new_frame, cv::Mat &old_frame, cv::Mat &output) {
	cv::absdiff(new_frame, old_frame, output);
}

//main methode handling searching for start sign from stop lights
void StopLightDetector::find_start(cv::Mat &input, cv::Mat &output, cv::Mat &old_input, int number) {
	cv::Mat roi;
	cv::Mat new_output;
	//because from first roi there is always change somehow
	if (number != 0) {
		//making specific roi
		make_roi(input, roi, number);
	}
	else {
		//not making roi
		input.copyTo(roi);
	}
	//getting only bright image
	split_bright(roi, new_output);
	//returning where bright object have changed
	give_difference(new_output, old_input, output);
	prepare_image(output, output);
	//giving info if there is sudden change in brightness
	count_pixels(output);
	//needed for next loop
	new_output.copyTo(old_input);
	
	//because from first roi there is always change somehow
	if (start_finding == false) {
		start_finding = true;
	}

}

//preparing first image
void StopLightDetector::prepare_first_image(cv::Mat &input, cv::Mat &output, int number) {
	cv::Mat roi;
	cv::Mat bright;
	if (number != 0)
		make_roi(input, roi, number);
	else 
		input.copyTo(roi);
	split_bright(roi, output);
}

//methot giving and setting varaibles that there is chane in lights
void StopLightDetector::count_pixels(cv::Mat &input) {
	int count = cv::countNonZero(input);
    //std::cout << count << std::endl;
    if (count < max_dif_pixels and count > min_dif_pixels and start_finding == true) {
		start_light = true;
		//std::cout << "FIND GREEN LIGHT" << std::endl;
	}
	else {
		start_light = false;
		//std::cout << "WAIT" << std::endl;
	}
	
}


