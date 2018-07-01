#ifndef IDS_H
#define IDS_H

#include <iostream>
#include <stdio.h>
#include <stddef.h>
#include "opencv2/opencv.hpp"
#include "ueye.h"

#include <pthread.h>

#define IDS_WIDTH 480
#define IDS_HEIGHT 752
#define IMAGE_COUNT 87
class IDS_PARAMETERS {

    typedef struct _UEYE_IMAGE
    {
        char *pBuf;
        INT nImageID;
        INT nImageSeqNum;
        INT nBufferSize;
    } UEYE_IMAGE, *PUEYE_IMAGE;

    HIDS m_hCamera;
    char *m_pLastBuffer;
    bool algorithm_ready = true;
    double meanfps = 0.0;
    UEYE_IMAGE m_Images[IMAGE_COUNT];

    void ProcessFrame ();
    void updateFps (double fps);

    void frameEvent(void);

    bool _AllocImages (int nWidth, int nHeight, int nBitspp);
    INT _GetImageNum (char* pbuf);
    void _FreeImages ();
    void _EmptyImages ();
    INT _GetImageID (char* pbuf);

    pthread_mutex_t signal_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t frame_signal = PTHREAD_COND_INITIALIZER;
public:

    cv::Mat ids_frame = cv::Mat(480, 752, CV_8UC3);
    pthread_mutex_t frame_mutex = PTHREAD_MUTEX_INITIALIZER;

    UINT PixelClock = 37;
    int pixelclock_slider = 37;
    double Exposure = 10.0;
    int exposure_slider = 100;
    double FPS = 87, NEWFPS;
    int fps_slider = 82;

    int Master_GAIN_Factor;//=300;
    int Red_GAIN_Factor;//=112;
    int Green_GAIN_Factor;//=100;
    int Blue_GAIN_Factor;//=158;
	
	int nRange[3];
    int sharpness_slider=1;
    UINT Sharpness=1;

    int Gamma =170;

    char* pMem = NULL;
    int memID = 0;

    void setAlgorithmReady();

    double getFPS();
    uchar* get_frame();
    HIDS getCameraHID();

    void frame_loop();
    void initialize_camera();
    void exit();
    void change_params();
    void setting_auto_params();
    void update_params();
    void create_trackbars(void);

};


#endif
