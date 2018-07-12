#include "kurokesu.hpp"

Kurokesu::Kurokesu()
{

}

bool Kurokesu::init()
{
    // Open communication with camera
    camera.open(CAMERA_INDEX, cv::CAP_V4L2);

    // Check if camera is opened propely
    if (!camera.isOpened())
    {
        std::cout << "Error could not open camera on port: " << CAMERA_INDEX << std::endl
                  << "Closing app!" << std::endl;

        camera.release();
        return 0;
    }
    else
    {
        // Set resolution
        camera.set(cv::CAP_PROP_FRAME_WIDTH, CAM_RES_X);
        camera.set(cv::CAP_PROP_FRAME_HEIGHT, CAM_RES_Y);

        // Set coding type of frames (Kurokesu)
        camera.set(cv::CAP_PROP_CONVERT_RGB, true);

        return 1;
    }
}

void Kurokesu::trackbars()
{
    cv::createTrackbar("Brightness", "CAM Settings", &this->cam_brightness, 64);
    cv::createTrackbar("Contrast", "CAM Settings", &this->cam_contrast, 64);
    cv::createTrackbar("Saturation", "CAM Settings", &this->cam_saturation, 128);
    cv::createTrackbar("Hue", "CAM Settings", &this->cam_hue, 40);
    cv::createTrackbar("White Bal Auto", "CAM Settings", &this->cam_white_bal_auto, 1);
    cv::createTrackbar("Gamma", "CAM Settings", &this->cam_gamma, 500);
    cv::createTrackbar("Gain", "CAM Settings", &this->cam_gain, 100);
    cv::createTrackbar("PWR freq", "CAM Settings", &this->cam_pwr_line_freq, 2);
    cv::createTrackbar("White Bal Auto", "CAM Settings", &this->cam_white_bal_temp, 9300);
    cv::createTrackbar("Sharpness", "CAM Settings", &this->cam_sharpness, 6);
    cv::createTrackbar("Backlight comp", "CAM Settings", &this->cam_backlight_compensation, 2);
    cv::createTrackbar("Exposure Auto", "CAM Settings", &this->cam_exposure_auto, 3);
    cv::createTrackbar("Exposure", "CAM Settings", &this->cam_exposure_absolute, 5000);
    cv::createTrackbar("Auto priority", "CAM Settings", &this->cam_auto_priority, 1);
}

void Kurokesu::update()
{
    std::cout << "v4l2-ctl -d /dev/video1 --set-ctrl=brightness=" + std::to_string(this->cam_brightness) << std::endl;
    system(("v4l2-ctl -d /dev/video1 --set-ctrl=brightness=" + std::to_string(this->cam_brightness)).c_str());
    system(("v4l2-ctl -d /dev/video1 --set-ctrl=contrast=" + std::to_string(this->cam_contrast)).c_str());
    system(("v4l2-ctl -d /dev/video1 --set-ctrl=saturation=" + std::to_string(this->cam_saturation)).c_str());
    system(("v4l2-ctl -d /dev/video1 --set-ctrl=hue=" + std::to_string(this->cam_hue)).c_str());
    system(("v4l2-ctl -d /dev/video1 --set-ctrl=white_balance_temperature_auto=" + std::to_string(this->cam_white_bal_auto)).c_str());
    system(("v4l2-ctl -d /dev/video1 --set-ctrl=gamma=" + std::to_string(this->cam_gamma)).c_str());
    system(("v4l2-ctl -d /dev/video1 --set-ctrl=gain=" + std::to_string(this->cam_gain)).c_str());
    system(("v4l2-ctl -d /dev/video1 --set-ctrl=power_line_frequency=" + std::to_string(this->cam_pwr_line_freq)).c_str());
    system(("v4l2-ctl -d /dev/video1 --set-ctrl=white_balance_temperature=" + std::to_string(this->cam_white_bal_temp)).c_str());
    system(("v4l2-ctl -d /dev/video1 --set-ctrl=sharpness=" + std::to_string(this->cam_sharpness)).c_str());
    system(("v4l2-ctl -d /dev/video1 --set-ctrl=backlight_compensation=" + std::to_string(this->cam_backlight_compensation)).c_str());
    system(("v4l2-ctl -d /dev/video1 --set-ctrl=exposure_auto=" + std::to_string(this->cam_exposure_auto)).c_str());
    system(("v4l2-ctl -d /dev/video1 --set-ctrl=exposure_absolute=" + std::to_string(this->cam_exposure_absolute)).c_str());
    system(("v4l2-ctl -d /dev/video1 --set-ctrl=exposure_auto_priority=" + std::to_string(this->cam_auto_priority)).c_str());
}
