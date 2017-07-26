#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <iostream>

using namespace std;
using namespace cv;

bool start_camera(VideoCapture camera, int camera_index, unsigned short camera_width, unsigned short camera_height);
void destroy_handlers();
bool set_CUDA_device(unsigned short CUDA_device_index);
Mat get_frame(VideoCapture camera);
Mat filter_frame(Mat frame, unsigned short compared_level_main, unsigned short ROI_horizontal_pos, unsigned short ROI_vertical_pos, unsigned short ROI_width, unsigned short ROI_height);
