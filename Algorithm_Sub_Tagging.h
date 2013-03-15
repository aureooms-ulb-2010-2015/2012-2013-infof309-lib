#ifndef ALGORITHM_SUB_TAGGING_H
#define ALGORITHM_SUB_TAGGING_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>

class Sub_Tagging{

public:
	typedef std::vector<cv::Rect> Rects;

private:

	cv::Mat _gray;			// current gray-level image
	cv::Mat _background;	// accumulated background
	cv::Mat backImage;		// background image
	cv::Mat foreground;		// foreground image

	std::vector<std::vector<cv::Point> > contour;
	std::vector<cv::Vec4i> hierarchy;
	cv::Rect _bndRect;
	cv::RNG rng;

	int threshold;			// threshold for foreground extraction
	double learningRate;    // learning rate in background accumulation

	cv::Size blurRange;

public:

	Sub_Tagging() : threshold(10), learningRate(0.1), blurRange(6,6){}
	Sub_Tagging(int threshold, double learningRate, cv::Size blurRange):
		threshold(threshold), learningRate(learningRate), blurRange(blurRange){}

	// Set the threshold used to declare a foreground
	void setThreshold(int t) {
		threshold = t;
	}

	// Set the learning rate
	void setLearningRate(double r) {
		learningRate = r;
	}

	// processing method
	Rects process(const cv::Mat &in) {
		// convert input frame to gray-level image and blur it
		cv::cvtColor(in, _gray, CV_BGR2GRAY);
		cv::blur( _gray, _gray, blurRange );

		// initialize background to 1st frame
		if (_background.empty()){
			_gray.convertTo(_background, CV_32F);// 32 bit floating point
		}

		// convert background to unsigned 8bit/pixel (values 0-255)
		_background.convertTo(backImage,CV_8U);

		// compute difference between current image and background
		cv::absdiff(backImage,_gray,foreground);

		// contrast the image
		cv::threshold(foreground,foreground,12,255,cv::THRESH_BINARY_INV);
		//CV_RETR_CCOMP
		//foreground = 255 - foreground;
		int erosion_type = cv::MORPH_RECT;
		int erosion_size = 1;
		cv::Mat element = cv::getStructuringElement( erosion_type, cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ), cv::Point( erosion_size, erosion_size ) );
		cv::morphologyEx(foreground,foreground,cv::MORPH_OPEN,element);
		cv::erode(foreground,foreground,cv::Mat());
		cv::dilate(foreground,foreground,cv::Mat());

		cv::findContours(foreground, contour, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );


		Rects rects;

		//Get & Draw the bounding rectangle around the moving object
		for(unsigned int i = 0; i< contour.size(); i++ ){
		   //has child or has no parent
		   //if ( hierarchy[i][2] != -1 || hierarchy[i][3] == -1 ) {
						// random colour
						//cv::Scalar colour( (rand()&255), (rand()&255), (rand()&255) );
			double area = cv::contourArea(contour.at(i));
			if( fabs(area) >= 10){
				rects.push_back(cv::boundingRect(contour.at(i)));
			}
		   //}
		}

		cv::Mat temp;
		// apply threshold to foreground image
		cv::threshold(foreground,temp,threshold,255,cv::THRESH_BINARY_INV);
		// accumulate background
		cv::accumulateWeighted(_gray, _background, learningRate, temp);

		return rects;
	}
};

#endif // ALGORITHM_SUB_TAGGING_H
