#include "process.hpp"

Process::Process()
{
    gap_pos_left = cv::Point(100, 100);
    gap_pos_right = cv::Point(200, 200);
}

void Process::simplify_data(LidarReading &points)
{
    cv::Point tmp, last;
    std::vector<cv::Point> tmp_vec_pos;
    std::vector<double> tmp_vec_angle;

    tmp_vec_pos.push_back(points.pos[0]);
    tmp_vec_angle.push_back(points.angle[0]);
    last = points.pos[0];

    for(uint32_t i = 1; i < points.pos.size() - 1; i++)
    {
        tmp = points.pos[i] - last;
        if(sqrt(tmp.x*tmp.x + tmp.y*tmp.y) > thresh_simplify)
        {
            tmp_vec_pos.push_back(points.pos[i]);
            tmp_vec_angle.push_back(points.angle[i]);
            last = points.pos[i];
        }
    }

    points.pos.clear();
    points.angle.clear();
    points.pos = tmp_vec_pos;
    points.angle = tmp_vec_angle;
}

void Process::split_poins(LidarReading &points)
{
    left_points.clear();
    right_points.clear();
    rejected_points.clear();

    uint32_t i;
    uint32_t j;

    if(points.pos.size() >= 4)
    {
        left_points.push_back(points.pos[points.pos.size()-1]);

        for(i = points.pos.size()-2; i > 0; i--)
        {
            if(sqrt((points.pos[i].x - points.pos[i+1].x)*(points.pos[i].x - points.pos[i+1].x) + (points.pos[i].y - points.pos[i+1].y)*(points.pos[i].y - points.pos[i+1].y)) < max_dist)
                left_points.push_back(points.pos[i]);
            else
                break;
        }

        right_points.push_back(points.pos[0]);

        for(j = 1; j < points.pos.size(); j++)
        {
            if(sqrt((points.pos[j].x - points.pos[j-1].x)*(points.pos[j].x - points.pos[j-1].x) + (points.pos[j].y - points.pos[j-1].y)*(points.pos[j].y - points.pos[j-1].y)) < max_dist)
                right_points.push_back(points.pos[j]);
            else
                break;
        }

        int i_int = i;
        int j_int= j;

        if(i_int - j_int >= 0)
        {
            for(uint32_t k = j; k <= i; k++)
                rejected_points.push_back(points.pos[k]);
        }
    }

}

void Process::split_poins_equally(LidarReading &points)
{
    left_points.clear();
    right_points.clear();
    rejected_points.clear();

    uint32_t i = 0;
    uint32_t j = points.pos.size();

    bool left_continous = true;
    bool right_continous = true;

    if(points.pos.size() >= 4)
    {
        if(points.pos[points.pos.size()-1].y > HEIGHT/2 - 50)
        {
            left_points.push_back(points.pos[points.pos.size()-1]);
        }
        else
        {
            left_continous = false;
        }

        if(points.pos[0].y > HEIGHT/2 - 50)
        {
            right_points.push_back(points.pos[0]);
        }
        else
        {
            right_continous = false;
        }

        std::cout << "Left:  " << points.angle[0] << std::endl;
        std::cout << "Right: " << points.angle[points.angle.size()-1] << std::endl;

        for(i = 1; i < points.pos.size()-1; i++)
        {
            if(right_continous)
            {
                if(sqrt((points.pos[i].x - points.pos[i-1].x)*(points.pos[i].x - points.pos[i-1].x) + (points.pos[i].y - points.pos[i-1].y)*(points.pos[i].y - points.pos[i-1].y)) < max_dist)
                    right_points.push_back(points.pos[i]);
                else
                    right_continous = false;
            }

            if(left_continous)
            {
                j = points.pos.size()-1-i;
                if(sqrt((points.pos[j].x - points.pos[j+1].x)*(points.pos[j].x - points.pos[j+1].x) + (points.pos[j].y - points.pos[j+1].y)*(points.pos[j].y - points.pos[j+1].y)) < max_dist)
                    left_points.push_back(points.pos[j]);
                else
                    left_continous = false;
            }

            if(points.pos.size()-1-i < i || (!left_continous && !right_continous))
                break;
        }

        int i_int = i;
        int j_int= j;

        if(j_int - i_int >= 0)
        {
            for(uint32_t k = i; k <= j; k++)
                rejected_points.push_back(points.pos[k]);
        }
    }

}

void Process::search_gap()
{
    if(left_points.size() > 0)
    {
        gap_pos_left = left_points[left_points.size()-1];
    }
    else
    {
        gap_pos_left = cv::Point(WIDTH/2 - 100, HEIGHT/2 - 100);
    }
    if(right_points.size() > 0)
    {
        gap_pos_right = right_points[right_points.size()-1];
    }
    else
    {
        gap_pos_right = cv::Point(WIDTH/2 + 100, HEIGHT/2 - 100);
    }
}

void Process::filter_enemies()
{
    int Px, Py, alpha;
    int Vx = gap_pos_left.x - gap_pos_right.x;
    int Vy = gap_pos_left.y - gap_pos_right.y;

    enemies_points.clear();
    trash_points.clear();

    for(uint32_t i = 0; i < rejected_points.size(); i++)
    {
        Px = rejected_points[i].x - gap_pos_right.x;
        Py = rejected_points[i].y - gap_pos_right.y;

        alpha = Vx*Py - Vy*Px;

        if(alpha < 0)
            enemies_points.push_back(rejected_points[i]);
        else
            trash_points.push_back(rejected_points[i]);
    }
}

void Process::draw_data(cv::Mat &out)
{
    for(uint32_t i = 0; i < left_points.size(); i++)
        cv::circle(out, left_points[i], 2, cv::Scalar(255, 0, 0), 2, 8, 0);

    for(uint32_t i = 0; i < right_points.size(); i++)
        cv::circle(out, right_points[i], 2, cv::Scalar(0, 255, 0), 2, 8, 0);

    for(uint32_t i = 0; i < enemies_points.size(); i++)
        cv::circle(out, enemies_points[i], 2, cv::Scalar(0, 255, 255), 2, 8, 0);

    for(uint32_t i = 0; i < trash_points.size(); i++)
        cv::circle(out, trash_points[i], 2, cv::Scalar(255, 0, 255), 2, 8, 0);

    cv::circle(out, gap_pos_left, 4, cv::Scalar(255, 255, 255), 2, 8, 0);
    cv::circle(out, gap_pos_right, 4, cv::Scalar(255, 255, 255), 2, 8, 0);
    cv::line(out, gap_pos_left, gap_pos_right, cv::Scalar(255,255,255), 2, cv::LINE_8, 0);
}
