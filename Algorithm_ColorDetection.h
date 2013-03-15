#ifndef ALGORITHM_COLORDETECTION_H
#define ALGORITHM_COLORDETECTION_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "FrameProcessor.h"


class ColorDetection : public FrameProcessor {

public:

    ColorDetection();
    void process(const cv::Mat &in, cv::Mat &out);
};

#endif // ALGORITHM_COLORDETECTION_H
