#include <opencv2/opencv.hpp>
#include "spline.hpp"
#include "main.h"
#include <vector>

extern int left_slider[5];
extern int right_slider[5];
extern int rect_slider[5];

void left_trackbar( int, void* );
void righ_trackbar( int, void* );
void init_trackbars();

void init_rect_trackbars(String name);
void rect_trackbars( int, void* );
void points_preview(std::vector<Point>points,Mat& frame,const Scalar& col);



