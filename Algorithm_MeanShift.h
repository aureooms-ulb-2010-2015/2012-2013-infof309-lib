#ifndef ALGORITHM_MEANSHIFT_H
#define ALGORITHM_MEANSHIFT_H

#include "FrameProcessor.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class MeanShift : public FrameProcessor {


    cv::Mat _input;

public:
    MeanShift(){}

    void process(const cv::Mat &in, cv::Mat &out){
        in.convertTo(_input, CV_8U);
        cv::pyrMeanShiftFiltering(_input,out,10,35);

    }
};

#endif // ALGORITHM_MEANSHIFT_H
