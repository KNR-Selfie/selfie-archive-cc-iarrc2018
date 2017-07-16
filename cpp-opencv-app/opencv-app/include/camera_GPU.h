#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <iostream>

using namespace std;
using namespace cv;

VideoCapture camera;

bool start_camera(int camera_index, unsigned short camera_width, unsigned short camera_height);
bool destroy_handlers(unsigned short camera_index);
bool set_CUDA_device(unsigned short CUDA_device_index);
Mat get_frame(unsigned short camera_index);
Mat filter_frame(Mat frame, unsigned short compared_level_main, unsigned short ROI_horizontal_pos, unsigned short ROI_vertical_pos, unsigned short ROI_width, unsigned short ROI_height);
