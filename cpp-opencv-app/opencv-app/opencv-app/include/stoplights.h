/************************************************************************
Project:
Selfie, automonous car
Authot:
Kornelia Lukojc
Descripiton:
Declaration of StopLightDetector class
*************************************************************************/

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

class StopLightDetector {
private:
	int max_dif_pixels = 1000;
	//makes roi on input image
	void make_roi(cv::Mat &input, cv::Mat &output, int number);
	//find bright
	void split_bright(cv::Mat &input, cv::Mat &output);
	//preparing image
	void prepare_image(cv::Mat &input, cv::Mat &output);
	//returning differences on image
	void give_difference(cv::Mat &new_frame, cv::Mat &old_frame, cv::Mat &output);
	//returning how many pixels have changed
	void count_pixels(cv::Mat &input);

public:
	//main flag telling us if green light is on image
	bool start_light = false;
	//flag not to analize first frame
	bool start_finding = false;
	//storing roi number
    int roi_number = 2;
	//prepare first image
	void prepare_first_image(cv::Mat &input, cv::Mat &output, int number);
	//showing roi rectangles
	void test_roi(cv::Mat &input, cv::Mat &output);
	//main method
	void find_start(cv::Mat &input, cv::Mat &output, cv::Mat &old_input, int number);


};

