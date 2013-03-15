#ifndef TARGET_H
#define TARGET_H

#include <string>
#include <opencv2/core/core.hpp>

class Target {
private:
    std::string _id;
    static uint32_t NEXT_ID;

public:
    int x;
    int y;
    double speed;
    double acceleration;
    cv::Scalar color;
    cv::Mat model;
	cv::Rect rect;
	int age;

	Target(cv::Rect);
	Target(int, int, cv::Scalar, cv::Mat);

    std::string getId() { return this->_id; }

};

#endif // TARGET_H
