#pragma once

//#define VID_MODE
#define IDS_MODE
#define DEBUG_MODE
//#define VERBOSE_MODE
//#define AUTO_THRESH
#define STOPLIGHTS_MODE
#define FPS_COUNT
//#define TEST_SHM

#define FRAME_TIME 1
#define FPS_AMOUNT 5000

#define CAMERA_INDEX 1

#define DETECTION_NUMBER 7

#define ROI_X_SEARCH 500
#define ROI_X 80
#define ROI_Y 40

#define MAX_STOP_ANGLE 50
#define ROI_STOP_X 30
#define ROI_STOP_Y 190

#ifndef DEBUG_MODE
#undef VERBOSE_MODE
#undef TEST_SHM
#endif

#define IDS_WIDTH 752
#define IDS_HEIGHT 360

#ifdef VID_MODE        // File
#define CAM_RES_X 640
#define CAM_RES_Y 360
#else
#ifndef IDS_MODE      // Kurokesu
#define CAM_RES_X 640
#define CAM_RES_Y 360
#else
#define CAM_RES_X IDS_WIDTH
#define CAM_RES_Y IDS_HEIGHT
#endif
#endif
