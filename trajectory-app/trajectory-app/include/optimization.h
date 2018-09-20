#include "main.h"
#include <opencv2/opencv.hpp>

void points_to_mat(Mat& point_mat,vector<Point> points); //get from shared memory
void one_line_planner(spline_t r_line,int offset, spline_t& path_line);
void two_line_planner(spline_t y_line, spline_t w_line, int offset, spline_t& path_line);
void new_optimization(vector<Point> pts_vector,spline_t& spl,Mat &preview);

//lidar version
void two_wall_planner(spline_t left_spline,spline_t right_spline, spline_t &path_line);
