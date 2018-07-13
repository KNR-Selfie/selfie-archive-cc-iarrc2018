#include "lanedetector.hpp"
#include <include/def.hpp>

cv::Mat tmp_mat(CAM_RES_Y, CAM_RES_X, CV_8UC3);

DetectionArea::DetectionArea()
{

}

void DetectionArea::init(cv::Point center)
{
    def_roi = cv::Rect(cv::Point(center.x-roi_x/2,center.y-ROI_Y/2),
                       cv::Point(center.x+roi_x/2,center.y+ROI_Y/2));
    roi = def_roi;
    middle_roi = center;
    middle_detection = center;
}

void DetectionArea::reset()
{
    roi_x = ROI_X_SEARCH;
    roi = def_roi;
    middle_roi = cv::Point(def_roi.x+def_roi.width/2,def_roi.y+def_roi.height/2);
}

void DetectionArea::update(cv::Mat &input)
{
    roi = cv::Rect(cv::Point(middle_roi.x-roi_x/2,middle_roi.y-ROI_Y/2),
                   cv::Point(middle_roi.x+roi_x/2,middle_roi.y+ROI_Y/2));
    frame_roi = input(roi);
}

void DetectionArea::find_center(int thresh)
{
    cv::reduce(frame_roi, frame_reduced, 0, CV_REDUCE_AVG);
    p = frame_reduced.ptr<uchar>(0);
    for (int i = 0; i < frame_reduced.cols; i++)
    {
        if(p[i] > thresh)
        {
            for(int k = frame_reduced.cols-1; k >= i; k--)
            {
                if(p[k] > thresh)
                {
                    left.x = i + middle_roi.x - roi.width/2;
                    right.x = k + middle_roi.x - roi.width/2;
                    middle_roi.x = (left.x + right.x)/2;
                    detected = true;
                    return;
                }
            }
        }
    }
    detected = false;
}

void DetectionArea::check_boundaries()
{
    middle_detection.x = middle_roi.x;

    if(middle_roi.x < ROI_X/2)
    {
        middle_roi.x = ROI_X/2;

        if(middle_roi.x <= 0)
            detected = false;
    }
    else if(middle_roi.x > CAM_RES_X - 1 - ROI_X/2)//CAM_RES_X/2+1)//CAM_RES_X - 1 - ROI_X/2)
    {
        middle_roi.x = CAM_RES_X - 1 - ROI_X/2;

        if(middle_roi.x >= CAM_RES_X - 1)
            detected = false;
    }
}

LaneDetector::LaneDetector()
{
    frame_stop.create(ROI_STOP_Y, ROI_STOP_X, CV_8UC1);
    frame_stop_reduced.create(ROI_STOP_Y, 1, CV_8UC1);
}

void LaneDetector::calculate_bird_var(cv::Mat &frame_ref, cv::Mat &frame_inv_ref)
{
    alpha = ((double)alpha_i - 90.)*CV_PI / 180;
    dist = (double)dist_i;
    dist_inv = (double)dist_inv_i;
    f = (double)f_i;

    taille = frame_ref.size();
    w = (double)taille.width, h = (double)taille.height;

    taille_inv = frame_inv_ref.size();
    w_inv = (double)taille_inv.width, h_inv = (double)taille_inv.height;

    A1 = (cv::Mat_<float>(4, 3) <<
        1, 0, -w / 2,
        0, 1, -h / 2,
        0, 0, 0,
        0, 0, 1);

    RX = (cv::Mat_<float>(4, 4) <<
        1, 0, 0, 0,
        0, cos(alpha), -sin(alpha), 0,
        0, sin(alpha), cos(alpha), 0,
        0, 0, 0, 1);

    R = RX;

    T = (cv::Mat_<float>(4, 4) <<
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, dist,
        0, 0, 0, 1);

    K = (cv::Mat_<float>(3, 4) <<
        f, 0, w / 2, 0,
        0, f, h / 2, 0,
        0, 0, 1, 0
        );    void pack_points();    void pack_points();

    A1_inv = (cv::Mat_<float>(4, 3) <<
        1, 0, -w_inv / 2,
        0, 1, -h_inv / 2,
        0, 0, 0,
        0, 0, 1);

    T_inv = (cv::Mat_<float>(4, 4) <<
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, dist_inv,
        0, 0, 0, 1);

    K_inv = (cv::Mat_<float>(3, 4) <<
        f, 0, w_inv / 2, 0,
        0, f, h_inv / 2, 0,
        0, 0, 1, 0
        );    void pack_points();    void pack_points();


    tmp_rect = cv::Rect(0, 0, CAM_RES_X, CAM_RES_Y - cut_y);

    transfo = K * (T * (R * A1));
    transfo_inv = K_inv * (T_inv * (R * A1_inv));
}

void LaneDetector::bird_eye(cv::Mat &input, cv::Mat &output)
{
    cv::warpPerspective(input, tmp_mat, transfo, taille, cv::INTER_CUBIC | cv::WARP_INVERSE_MAP);
    tmp_mat(tmp_rect).copyTo(output);
}

void LaneDetector::bird_eye_inverse(cv::Mat &input, cv::Mat &output)
{
    cv::warpPerspective(input, output, transfo_inv, taille_inv, cv::INTER_CUBIC);
}

void LaneDetector::binarize(cv::Mat &input, cv::Mat &output, int &thresh_low, int &thresh_high)
{
    cv::inRange(input, thresh_low, thresh_high, output);
}

void LaneDetector::opening(cv::Mat &input, cv::Mat &output)
{
    cv::erode(input, tmp_mat, element_erosion);
    cv::dilate(tmp_mat, output, element_dilation);
}

void LaneDetector::closing(cv::Mat &input, cv::Mat &output)
{
    cv::dilate(input, tmp_mat, element_dilation);
    cv::erode(tmp_mat, output, element_erosion);
}

void LaneDetector::apply_sobel(cv::Mat &input, cv::Mat &output)
{
    cv::Sobel(input, output, CV_8U, 2, 0, 3, 1, 0, cv::BORDER_DEFAULT);
}

void LaneDetector::correct_ROIs(DetectionArea input[], cv::Mat &frame)
{
    if(!input[0].detected)
    {
        if(input[1].detected)
        {
            input[0].middle_roi.x = input[1].middle_roi.x;
            input[0].update(frame);
        }
    }

    for(int i = 1; i < DETECTION_NUMBER - 1; i++)
    {
        if(input[i-1].detected)
        {
            if(input[i+1].detected)
            {
                input[i].middle_roi.x = (input[i-1].middle_roi.x + input[i+1].middle_roi.x)/2;
                input[i].update(frame);
            }
            else
            {
                input[i].middle_roi.x = input[i-1].middle_roi.x;
                input[i].update(frame);
            }
        }else if(input[i+1].detected)
        {
            input[i].middle_roi.x = input[i+1].middle_roi.x;
            input[i].update(frame);
        }
    }

    if(!input[DETECTION_NUMBER - 1].detected)
    {
        if(input[DETECTION_NUMBER - 2].detected)
        {
            input[DETECTION_NUMBER - 1].middle_roi.x = input[DETECTION_NUMBER - 2].middle_roi.x;
            input[DETECTION_NUMBER - 1].update(frame);
        }
    }
}

void LaneDetector::calculate_thresh(cv::Mat &frame, DetectionArea area_line[])
{
    for(int i = DETECTION_NUMBER-1; i >= 0; i--)
    {
        tmp[i] = cv::Rect(area_line[i].roi.x + ROI_X, area_line[i].roi.y, CAM_RES_X-1 - ROI_X - area_line[i].roi.x, ROI_Y);
        roi_thresh = cv::Mat(frame, tmp[i]);
        std::cout << i << ": " << cv::mean(roi_thresh)[0] << std::endl;
    }
    std::cout << std::endl;
}

void LaneDetector::search_stop_line(DetectionArea area_line[], cv::Mat &input, int thresh_stop)
{
    if(area_line[0].detected && area_line[DETECTION_NUMBER -2].detected)
    {
        if(abs(area_line[0].middle_roi.x - area_line[DETECTION_NUMBER -2].middle_roi.x) < MAX_STOP_ANGLE)
        {
            if(area_line[0].middle_roi.x > area_line[DETECTION_NUMBER -2].middle_roi.x)
            {
                roi_stop = cv::Rect(area_line[0].middle_roi.x + MAX_STOP_ANGLE + 10, area_line[DETECTION_NUMBER-2].middle_roi.y - 60, ROI_STOP_X, ROI_STOP_Y);
            }
            else
            {
                roi_stop = cv::Rect(area_line[DETECTION_NUMBER -2].middle_roi.x + MAX_STOP_ANGLE + 10, area_line[DETECTION_NUMBER-2].middle_roi.y - 60, ROI_STOP_X, ROI_STOP_Y);
            }

            if(roi_stop.x + roi_stop.width < CAM_RES_X-1)
            {
                display_stop_roi = true;

                frame_stop = input(roi_stop);
                cv::reduce(frame_stop, frame_stop_reduced, 1, CV_REDUCE_AVG);

                for (int i = frame_stop_reduced.rows; i >= 0; i--)
                {
                    if(frame_stop_reduced.at<uchar>(0,i) > thresh_stop)
                    {
                            stop_distance = i;
                            stop_detected = true;
                            std::cout << "STOP DETECTED: " << ROI_STOP_Y - stop_distance << std::endl;
                            return;
                    }
                }
                stop_detected = false;
            }
            else
            {
                display_stop_roi = false;
#ifdef VERBOSE_MODE
                std::cout << "Not searching stop line" << std::endl;
#endif
            }
        }
        else
        {
            display_stop_roi = false;
#ifdef VERBOSE_MODE
            std::cout << "Not searching stop line" << std::endl;
#endif
        }
    }
    else
    {
        display_stop_roi = false;
#ifdef VERBOSE_MODE
            std::cout << "Not searching stop line" << std::endl;
#endif
    }
}

void LaneDetector::draw_data(cv::Mat &input, DetectionArea area_line[])
{
    if(display_stop_roi)
    {
        if(stop_detected)
        {
            cv::rectangle(input, roi_stop, cv::Scalar(0,255,0), 2, cv::LINE_8, 0);
        }
        else
        {
            cv::rectangle(input, roi_stop, cv::Scalar(0,0,255), 2, cv::LINE_8, 0);
        }
    }

    for(int i = 0; i < DETECTION_NUMBER; i++)
    {
        if(area_line[i].detected)
        {
            cv::circle(input, area_line[i].middle_detection, 2, cv::Scalar(0,255,0), CV_FILLED, cv::LINE_AA);
            cv::rectangle(input, area_line[i].roi, cv::Scalar(0,255,0), 2, cv::LINE_8, 0);
        }
        else
        {
            cv::circle(input, area_line[i].middle_detection, 2, cv::Scalar(0,255,0), CV_FILLED, cv::LINE_AA);
            cv::rectangle(input, area_line[i].roi, cv::Scalar(0,0,255), 2, cv::LINE_8, 0);
        }

    }

    for(int i = 0; i < DETECTION_NUMBER; i++)
    {
        cv::rectangle(input, tmp[i], cv::Scalar(256,128,0), 2);
    }
}

void LaneDetector::pack_data(DetectionArea area_line[], std::vector<cv::Point> &left_points)
{
    left_points.clear();

    for(int i = 0; i < DETECTION_NUMBER; i++)
    {
        if(area_line[i].detected)
        {
            left_points.push_back(area_line[i].middle_detection);
        }
    }
}
