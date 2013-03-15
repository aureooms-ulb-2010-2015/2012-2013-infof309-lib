#include "Target.h"
#include <sstream>

uint32_t Target::NEXT_ID = ~0;

Target::Target(cv::Rect rect):rect(rect),age(0){}

Target::Target(int x, int y, cv::Scalar color, cv::Mat model):x(x),y(y),speed(0),acceleration(0),color(color),model(model),age(0){
    std::ostringstream stream;
    stream << ++NEXT_ID;
    this->_id = stream.str();
}
