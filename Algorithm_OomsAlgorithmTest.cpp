#include "Algorithm_OomsAlgorithmTest.h"

OomsAlgorithmTest::OomsAlgorithmTest():_learningRate(0.01),_threshold(10){}
OomsAlgorithmTest::OomsAlgorithmTest(double learningRate, int threshold):_learningRate(learningRate),_threshold(threshold){}

void OomsAlgorithmTest::process(const cv::Mat &in, cv::Mat &out) {
	out = in.clone();
	//return;
	// convert input frame to gray-level image and blur it
	cv::cvtColor(out, _gray, CV_BGR2GRAY);
	cv::blur( _gray, _gray, cv::Size(6,6) );

	// initialize background to 1st frame
	if (_background.empty()){
		_gray.convertTo(_background, CV_32F);// 32 bit floating point
	}

	// convert background to unsigned 8bit/pixel (values 0-255)
	_background.convertTo(_backImage, CV_8U);

	// compute difference between current image and background
	cv::absdiff(_backImage, _gray, _foreground);

	// contrast the image
	cv::threshold(_foreground, _foreground, 12, 255, cv::THRESH_BINARY_INV);

	cv::findContours(_foreground, _contour, _hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
	cv::Scalar color = cv::Scalar(123, 60, 42);


	//Get & Draw the bounding rectangle around the moving object
	for(unsigned int i = 0; i< _contour.size(); i++ ){
		_bndRect = cv::boundingRect(_contour.at(i));
		cv::rectangle(out, _bndRect, color, 1, 8, 0 );
	}

	cv::Mat temp;
	// apply threshold to foreground image
	cv::threshold(_foreground, temp, _threshold, 255, cv::THRESH_BINARY_INV);
	// accumulate background
	cv::accumulateWeighted(_gray, _background, _learningRate, temp);
}
