#ifndef ALGORITHM_CUSTOMCONDENSATION_H
#define ALGORITHM_CUSTOMCONDENSATION_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "FrameProcessor.h"

#include <random>

template<typename M> class CustomCondensation: public FrameProcessor{
protected:
	M matcher;

public:
	void process(const cv::Mat &in, cv::Mat &out) = 0;
};

#endif // ALGORITHM_CUSTOMCONDENSATION_H
