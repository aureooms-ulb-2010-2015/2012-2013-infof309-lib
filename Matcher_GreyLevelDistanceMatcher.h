#ifndef MATCHER_GREYLEVELDISTANCEMATCHER_H
#define MATCHER_GREYLEVELDISTANCEMATCHER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>
#include "Matcher_DistanceMatcher.h"

class GreyLevelDistanceMatcher : public DistanceMatcher{


public:
	typedef DistanceMatcher::Distance Distance;
	GreyLevelDistanceMatcher(int radius):DistanceMatcher(radius){}

	virtual ~GreyLevelDistanceMatcher() throw(){}
	virtual Distance computeDistance(const cv::Mat& prev_img, const cv::Mat& img,const cv::Point& a, const cv::Point& b){
		cv::Rect roi[2];
		roi[0].x = std::max(a.x - R, 0);
		roi[0].y = std::max(a.y - R, 0);
		roi[0].width  = (a.x + R + 1 > img.cols)? R + img.cols - a.x : R*2+1;
		roi[0].height = (a.y + R + 1> img.rows)? R + img.rows - a.y : R*2+1;
		roi[1].x = std::max(b.x - R, 0);
		roi[1].y = std::max(b.y - R, 0);
		roi[1].width  = (b.x + R + 1> img.cols)? R + img.cols - b.x : R*2+1;
		roi[1].height = (b.y + R + 1> img.rows)? R + img.rows - b.y : R*2+1;

		roi[0].width  =  roi[1].width = std::min( roi[0].width,  roi[1].width);
		roi[0].height = roi[1].height = std::min(roi[0].height, roi[1].height);

		cv::Mat grey[2], diff;
		//std::cout << "x : " << roi[0].x  <<", y : " << roi[0].y  <<", width : " << roi[0].width  <<", height : " << roi[0].height  << std::endl;
		//std::cout << "x : " << roi[1].x  <<", y : " << roi[1].y  <<", width : " << roi[1].width  <<", height : " << roi[1].height  << std::endl;
		grey[0] = prev_img(roi[0]);
		grey[1] = img(roi[1]);

		cv::absdiff(grey[0], grey[1], diff);

		Distance distance = 0;
		for(int i = 0; i < diff.rows; ++i){
			for(int j = 0; j < diff.cols; ++j){
				distance += diff.ptr(i)[j]*diff.ptr(i)[j];
			}
		}
		return sqrt(distance);
	}
};

#endif // MATCHER_GREYLEVELDISTANCEMATCHER_H
