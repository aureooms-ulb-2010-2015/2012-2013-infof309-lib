#ifndef ALGORITHM_NOPROCESSING_H
#define ALGORITHM_NOPROCESSING_H


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "FrameProcessor.h"


class NoProcessing : public FrameProcessor {

public:

	NoProcessing();
	void process(const cv::Mat &in, cv::Mat &out);
};


#endif // ALGORITHM_NOPROCESSING_H
