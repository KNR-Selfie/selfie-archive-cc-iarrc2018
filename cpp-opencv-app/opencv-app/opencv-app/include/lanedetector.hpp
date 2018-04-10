#pragma once

class LaneDetector
{
public:
    LaneDetector();
    void applyBlur();
    void colorTransform();
    void edgeDetect();
    void detectLine();
    void drawData();
};
