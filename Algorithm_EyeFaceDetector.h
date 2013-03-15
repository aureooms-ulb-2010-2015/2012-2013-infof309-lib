#ifndef ALGORITHM_EYEFACEDETECTOR_H
#define ALGORITHM_EYEFACEDETECTOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/contrib/detection_based_tracker.hpp>
#include <iostream>
#include "FrameProcessor.h"

class EyeFaceDetector : public FrameProcessor {

private:
    cv::CascadeClassifier _classifier;
    cv::CascadeClassifier _eyeClassifier;
public:
    EyeFaceDetector();
    void process(const cv::Mat &in, cv::Mat &out);
    //bool EyeFaceDetector::recognize(cv::Mat img1, cv::Mat img2);
};

#endif // ALGORITHM_EYEFACEDETECTOR_H
