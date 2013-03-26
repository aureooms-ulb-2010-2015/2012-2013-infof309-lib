#ifndef MATCHER_DISTANCEMATCHER_H
#define MATCHER_DISTANCEMATCHER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class DistanceMatcher{

protected:
	int R;

public:
	typedef unsigned int Distance;
	DistanceMatcher(int radius):R(radius){}
	virtual ~DistanceMatcher() throw(){}
	virtual void setRadius(unsigned int radius){ R = radius;}
	virtual Distance computeDistance(const cv::Mat&, const cv::Mat&,const cv::Point&, const cv::Point&) = 0;
};

#endif // MATCHER_DISTANCEMATCHER_H
