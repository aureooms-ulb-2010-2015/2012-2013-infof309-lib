#include "Matcher_TargetMatcherV2_BinaryMask.h"
#include <iostream>
#include <algorithm>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

TargetMatcherV2_BinaryMask::TargetMatcherV2_BinaryMask(){
}

TargetMatcherV2_BinaryMask::~TargetMatcherV2_BinaryMask() throw(){}

int TargetMatcherV2_BinaryMask::computeScore(const Target::Data &a, const Target::Data &b){
	int cols = std::min(a.cols, b.cols);
	int rows = std::min(a.rows, b.rows);
	cv::Rect roi_a((a.cols - cols)/2, (a.rows - rows)/2, cols, rows);
	cv::Rect roi_b((b.cols - cols)/2, (b.rows - rows)/2, cols, rows);
	cv::Mat grey_a, grey_b, diff;
	cv::cvtColor(a(roi_a), grey_a, CV_BGR2GRAY);
	cv::cvtColor(b(roi_b), grey_b, CV_BGR2GRAY);
	cv::blur(grey_a, grey_a, cv::Size(16,16) );
	cv::blur(grey_b, grey_b, cv::Size(16,16) );
	cv::absdiff(grey_a,grey_b,diff);

	int score = 0;
	for(int i = 0; i < diff.rows; ++i){
		for(int j = 0; j < diff.cols; ++j){
			if(i > diff.rows/4 && i <= diff.rows/4*3 && j > diff.cols/4 && j <= diff.cols/4*3){
				score -= diff.ptr(i)[j] * 3;
			}
			else{
				score -= diff.ptr(i)[j];
			}
		}
	}

	//std::cout << c << std::endl;
	size_t k = diff.rows*diff.cols;
	return (100+(double)score/k) * 1000000;
}
