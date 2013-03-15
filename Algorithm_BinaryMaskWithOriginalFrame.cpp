#include "Algorithm_BinaryMaskWithOriginalFrame.h"

BinaryMaskWithOriginalFrame::BinaryMaskWithOriginalFrame(){
}

void BinaryMaskWithOriginalFrame::process(const cv::Mat &in, cv::Mat &out) {
	this->_mask.process(in,out);
	cv::Mat temp;
	// scale input image, result in temp (null size, we only use scale factors here)
	cv::resize(in, temp, cv::Size(), 0.2, 0.2, cv::INTER_CUBIC);

	// define destination for resized image
	cv::Rect roi( cv::Point( 0, 0 ), temp.size());
	cv::Mat destination = out(roi);

	// copy resized image to destination in out
	temp.copyTo(destination);
	return;
}
