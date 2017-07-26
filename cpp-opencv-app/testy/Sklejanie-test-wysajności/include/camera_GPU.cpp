#include "camera_GPU.h"

bool start_camera(VideoCapture camera, int camera_index, unsigned short camera_width, unsigned short camera_height)
{
    camera.open(camera_index);
    
    if (camera.isOpened())
    {
        cout << "Camera opened properly on port 0" << endl;
	cout << "Resolution: " << camera.get(CV_CAP_PROP_FRAME_WIDTH) << " x " << camera.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
		
        return 1;
    }	
    else
    {
		cout << "ERROR: Could not access camera on port 0" << endl;
		
        return 0;
    }
}

void destroy_handlers()
{
	camera.release();
    gpu::resetDevice();
}

bool set_CUDA_device(unsigned short CUDA_device_index)
{
    //Check of apropriate GPU module with CUDA enabled
    if(!gpu::getCudaEnabledDeviceCount())
    {
		cout << "No CUDA module detected!" << endl;
		cout << "Closing app!" << endl;
		return 0;
    }
    else
    {
		cout << "Detected " << gpu::getCudaEnabledDeviceCount() << " CUDA device/s" << endl;
    }
    
    //Setting of GPU module on adress 0
    gpu::setDevice(CUDA_device_index);

    //Check of proper initializing of CUDA device on adress 0
    gpu::DeviceInfo CUDA_device;
    if(CUDA_device.deviceID() == 0 && CUDA_device.isCompatible() == true)
    {
		cout << "CUDA device initialized properly on adress 0" << endl;
    }
    else
    {
		cout << "Initializing CUDA device on adress 0 failed!" << endl;
		cout << "Closing app!" << endl;
		return 0;
    }

    //Display info about CUDA device
	cout << "CUDA device name: " << CUDA_device.name() << endl;
	cout << "Version: " << CUDA_device.majorVersion() << "." << CUDA_device.minorVersion() << endl;
	cout << "Number of streaming multiprocessors: " << CUDA_device.multiProcessorCount() << endl;
	cout << "Total device GDDR5 memory: " << CUDA_device.totalMemory() << " bytes" << endl;
	cout << "Amount of free GDDR5 memory: " << CUDA_device.freeMemory() << " bytes" << endl;
	
	return 1;
}

Mat get_frame(unsigned short camera_index)
{
        Mat frame;	

        camera >> frame;
	
	if(!frame.data)
	{
		cout << "No image data!" << endl;
	}
	
	return frame;
}

Mat filter_frame(Mat frame, unsigned short compared_level_main, unsigned short ROI_horizontal_pos, unsigned short ROI_vertical_pos, unsigned short ROI_width, unsigned short ROI_height)
{
	Mat frame_ROI, frame_from_gpu;
	gpu::GpuMat gpu_frame, gpu_hsv, gpu_range_frame, gpu_filtered;
	vector<gpu::GpuMat> gpu_hsv_split;
	
	int erosion_size = 1;  
	Mat element = getStructuringElement(
		cv::MORPH_RECT, 
		cv::Size(2 * erosion_size, 2 * erosion_size), 
		cv::Point(erosion_size, erosion_size) );

	Rect ROI_rec(
		ROI_horizontal_pos,
		ROI_vertical_pos,
		ROI_width,
		ROI_height);

	frame(ROI_rec).copyTo(frame_ROI);

	gpu_frame.upload(frame_ROI);

	gpu::cvtColor(gpu_frame, gpu_hsv, CV_BGR2HSV);
	gpu::split(gpu_hsv, gpu_hsv_split);
	gpu::threshold(gpu_hsv_split[2], gpu_range_frame, compared_level_main, 255, THRESH_BINARY);
	dilate(gpu_range_frame, gpu_filtered, element);

	gpu_filtered.download(frame_from_gpu);
	
	return frame_from_gpu;
}
