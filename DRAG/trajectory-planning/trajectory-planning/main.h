#ifndef MAIN_H
#define MAIN_H


#include "include/spline.hpp"
#include "include/sliders.h"
#include "include/tangent.hpp"
#include "include/sharedmemory.hpp"
#include "include/interpolation.h"
#include "include/usb.hpp"
#include "include/optimization.h"
#include "include/file.h"


#include <numeric>
#include <algorithm>
#include <fstream>
#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>



//#define RACE_MODE //without display
#define DEBUG_MODE //data display
//#define TEST_MODE //load data from file

extern const int Height;
extern const int Width;

extern int number_of_rec_cols;
extern int number_of_rec_raws;

#endif // MAIN_H
