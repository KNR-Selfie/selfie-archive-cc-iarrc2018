#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <iostream>

using namespace std;
using namespace cv;

VideoCapture camera;

bool start_camera(unsigned short camera_index, unsigned short camera_width, unsigned short camera_height);
