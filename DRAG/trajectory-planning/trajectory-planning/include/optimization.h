#include "main.h"
#include <opencv2/opencv.hpp>

bool rectangle_optimize(Mat& point_mat,spline_t& spl);
void points_to_mat(Mat& point_mat,vector<Point> points); //get from shared memory
void one_line_planner(spline_t r_line,int offset, spline_t& path_line);
void line_search(int previous_angle, spline_t &path_line);
void two_line_planner(spline_t y_line, spline_t w_line, int offset, spline_t& path_line);
void drag_optimization(vector<Point> y_point_vector,spline_t &trajectory_path,int drag_offset);
bool line_side(vector<Point> point_vector);
int auto_offset(vector<Point> pts);

