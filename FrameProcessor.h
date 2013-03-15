#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H

#include <opencv2/core/core.hpp>

// The processor interface
template<typename T>
class Processor{

public:
	//process method
	virtual void process(const T& input, T& output) = 0;
	virtual ~Processor(){}

};

// The frame processor interface
class FrameProcessor : public Processor<cv::Mat>{
public:
	virtual ~FrameProcessor(){}
};

#endif // FRAMEPROCESSOR_H
