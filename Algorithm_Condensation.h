#ifndef ALGORITHM_CONDENSATION_H
#define ALGORITHM_CONDENSATION_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/legacy/legacy.hpp>

#include "FrameProcessor.h"

class Condensation : public FrameProcessor {
private:
	std::vector<cv::Point> mouseV, particleV;
	cv::Mat_<float> measurement;
	CvMat LB, UB;
	CvConDensation* condens;
	float xRange, yRange;

public:

	Condensation();
	void process(const cv::Mat &in, cv::Mat &out);
};

#endif // ALGORITHM_CONDENSATION_H
