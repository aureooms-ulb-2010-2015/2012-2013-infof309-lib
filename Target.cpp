#include "Target.h"
#include <sstream>

Target::Target(cv::Rect rect):rect(rect),age(0){}

Target::Target(int x, int y, cv::Scalar color, cv::Mat model):x(x),y(y),speed(0),acceleration(0),color(color),model(model),age(0){}
