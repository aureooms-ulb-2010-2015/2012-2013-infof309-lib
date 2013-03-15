#ifndef TARGET_CONDENSATIONTARGETV1_H
#define TARGET_CONDENSATIONTARGETV1_H

#include "Target_CondensationTarget.h"
#include <opencv2/highgui/highgui.hpp>

class CondensationTargetV1 : public CondensationTarget{

public:

	typedef cv::Mat Data;

	Data picture;

	CondensationTargetV1(const cv::Mat& in, cv::Rect r) : CondensationTarget(cv::Size(in.cols, in.rows), r), picture(in(r)){}

	CondensationTargetV1(cv::Size size, Data p, cv::Rect r) : CondensationTarget(size, r), picture(p){}

	CondensationTargetV1(cv::Size size, Data p, cv::Rect r, Densities s):
		CondensationTarget(size, r, s), picture(p){}

	CondensationTargetV1(const cv::Mat& in, cv::Rect r, Densities s) :
		CondensationTarget(cv::Size(in.cols, in.rows), r, s),picture(in(r)){}

};

#endif // TARGET_CONDENSATIONTARGETV1_H
