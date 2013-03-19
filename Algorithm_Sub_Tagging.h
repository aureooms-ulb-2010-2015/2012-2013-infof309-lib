#ifndef ALGORITHM_SUB_TAGGING_H
#define ALGORITHM_SUB_TAGGING_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
#include "Algorithm_AccumulateBackground.h"

class Sub_Tagging{

public:
	typedef std::vector<cv::Rect> Rects;

private:


	std::vector<std::vector<cv::Point> > contour;
	std::vector<cv::Vec4i> hierarchy;
	AccumulateBackground accumulator;

public:

	Sub_Tagging(){}
	Sub_Tagging(int threshold, double learningRate, cv::Size blurRange):
		accumulator(threshold,learningRate, blurRange){}

	// processing method
	Rects process(const cv::Mat &in) {
		accumulator.process(in);

		cv::findContours(accumulator.getForeground(),
						 contour,
						 hierarchy,
						 CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );


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

		return rects;
	}
};

#endif // ALGORITHM_SUB_TAGGING_H
