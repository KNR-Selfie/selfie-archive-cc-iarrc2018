#include "process.hpp"

Process::Process()
{
    gap_pos_left = cv::Point(100, 100);
    gap_pos_right = cv::Point(200, 200);
}

void Process::simplify_data(std::vector<cv::Point> &points)
{
    cv::Point tmp, last;
    std::vector<cv::Point> tmp_vec;

    tmp_vec.push_back(points[0]);
    last = points[0];

    for(uint32_t i = 1; i < points.size() - 1; i++)
    {
        tmp = points[i] - last;
        if(sqrt(tmp.x*tmp.x + tmp.y*tmp.y) > thresh_simplify)
        {
            tmp_vec.push_back(points[i]);
            last = points[i];
        }
    }

    points.clear();
    points = tmp_vec;
}

}
}
