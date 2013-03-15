#ifndef OOMSALGORITHMTEST_H
#define OOMSALGORITHMTEST_H


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "FrameProcessor.h"


class OomsAlgorithmTest : public FrameProcessor {

private:

	cv::Mat _gray;			// current gray-level image
	cv::Mat _background;	// accumulated background
	cv::Mat _backImage;		// background image
	cv::Mat _foreground;		// foreground image

	std::vector<std::vector<cv::Point> > _contour;
	std::vector<cv::Vec4i> _hierarchy;
	cv::Rect _bndRect;
	cv::RNG _rng;

	double _learningRate;    // learning rate in background accumulation
	int _threshold;			// threshold for foreground extraction

public:

	OomsAlgorithmTest();
	OomsAlgorithmTest(double learningRate, int threshold);

	// processing method
	void process(const cv::Mat &in, cv::Mat &out);
};

#endif // OOMSALGORITHMTEST_H
