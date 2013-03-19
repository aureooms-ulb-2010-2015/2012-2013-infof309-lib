#ifndef ALGORITHM_ACCUMULATEBACKGROUND_H
#define ALGORITHM_ACCUMULATEBACKGROUND_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>

class AccumulateBackground{

public:
	typedef std::vector<cv::Rect> Rects;

private:

	cv::Mat _grey;			// current gray-level image
	cv::Mat _background;	// accumulated background
	cv::Mat backgroundImage;// background image
	cv::Mat foreground;		// foreground image


	int threshold;			// threshold for foreground extraction
	double learningRate;    // learning rate in background accumulation

	cv::Size blurRange;

	int erosion_type = cv::MORPH_RECT;
	int erosion_size = 1;

public:

	AccumulateBackground() : threshold(10), learningRate(0.1), blurRange(6,6){}
	AccumulateBackground(int threshold, double learningRate, cv::Size blurRange):
		threshold(threshold), learningRate(learningRate), blurRange(blurRange){}

	const cv::Mat& getForeground(){
		return foreground;
	}

	// processing method
	void process(const cv::Mat &in) {
		// convert input frame to gray-level image and blur it
		cv::cvtColor(in, _grey, CV_BGR2GRAY);
		cv::blur( _grey, _grey, blurRange );

		// initialize background to 1st frame
		if (_background.empty()){
			_grey.convertTo(_background, CV_32F);// 32 bit floating point
		}

		// convert background to unsigned 8bit/pixel (values 0-255)
		_background.convertTo(backgroundImage,CV_8U);

		// compute difference between current image and background
		cv::absdiff(backgroundImage,_grey,foreground);

		// contrast the image
		cv::threshold(foreground,foreground,12,255,cv::THRESH_BINARY_INV);
		//CV_RETR_CCOMP
		//foreground = 255 - foreground;
		cv::Mat element = cv::getStructuringElement( erosion_type, cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ), cv::Point( erosion_size, erosion_size ) );
		cv::morphologyEx(foreground,foreground,cv::MORPH_OPEN,element);
		cv::erode(foreground,foreground,cv::Mat());
		cv::dilate(foreground,foreground,cv::Mat());

		cv::Mat temp;
		// apply threshold to foreground image
		cv::threshold(foreground,temp,threshold,255,cv::THRESH_BINARY_INV);
		// accumulate background
		cv::accumulateWeighted(_grey, _background, learningRate, temp);
	}
};
#endif // ALGORITHM_ACCUMULATEBACKGROUND_H
