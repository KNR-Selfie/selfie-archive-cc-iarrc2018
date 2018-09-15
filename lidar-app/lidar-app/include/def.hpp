#pragma once

#define DEBUG_MODE
//#define VERBOSE_MODE
#define SHM_TEST
#define FPS_COUNT

#define FRAME_TIME 1
#define FPS_AMOUNT 5000

#define WIDTH 1600
#define HEIGHT 2000

#ifndef DEBUG_MODE
#undef VERBOSE_MODE
#endif
