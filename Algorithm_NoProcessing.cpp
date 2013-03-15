#include "Algorithm_NoProcessing.h"

NoProcessing::NoProcessing(){}

void NoProcessing::process(const cv::Mat &in, cv::Mat &out) {
	out = in;
	return;
}
