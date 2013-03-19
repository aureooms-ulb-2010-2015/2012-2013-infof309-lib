#ifndef MATCHER_GREYLEVELDISTANCEMATCHER_H
#define MATCHER_GREYLEVELDISTANCEMATCHER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>

template<size_t D>class GreyLevelDistanceMatcher{
public:
	typedef CondensationTargetV2 Target;
	typedef size_t Distance;
	GreyLevelDistanceMatcher(){}
	virtual ~GreyLevelDistanceMatcher() throw(){}
	virtual Distance computeDistance(const cv::Mat& img, const cv::Point& a, const cv::Point& b){
		cv::Rect roi[2];
		roi[0].x = std::min(a.x - D, 0);
		roi[0].y = std::min(a.y - D, 0);
		roi[0].width  = (a.x + D > img.cols)? D + 1 + img.cols - a.x : D*2+1;
		roi[0].height = (a.y + D > img.rows)? D + 1 + img.rows - a.y : D*2+1;
		roi[1].x = std::min(b.x - D, 0);
		roi[1].y = std::min(b.y - D, 0);
		roi[1].width  = (b.x + D > img.cols)? D + 1 + img.cols - b.x : D*2+1;
		roi[1].height = (b.y + D > img.rows)? D + 1 + img.rows - b.y : D*2+1;

		cv::Mat grey[2], diff;

		cv::cvtColor(img(roi[0]), grey[0], CV_BGR2GRAY);
		cv::cvtColor(img(roi[1]), grey[1], CV_BGR2GRAY);

		cv::absdiff(grey[0], grey[1], diff);

		size_t distance = 0;
		for(int i = 0; i < diff.rows; ++i){
			for(int j = 0; j < diff.cols; ++j){
				distance += diff.ptr(i)[j];
			}
		}
		return distance;
	}
};

#endif // MATCHER_GREYLEVELDISTANCEMATCHER_H
