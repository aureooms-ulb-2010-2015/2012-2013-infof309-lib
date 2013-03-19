#ifndef TARGET_H
#define TARGET_H

#include <string>
#include <opencv2/core/core.hpp>

class Target {

public:
    int x;
    int y;
    double speed;
    double acceleration;
    cv::Scalar color;
    cv::Mat model;
	cv::Rect rect;
	int age;
	size_t tid;

	Target(cv::Rect);
	Target(int, int, cv::Scalar, cv::Mat);

};

#endif // TARGET_H
