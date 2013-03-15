#ifndef STREAMIMPL_H
#define STREAMIMPL_H
#include "Stream.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class StreamImpl : public Stream<cv::Mat>
{
private:
	cv::VideoCapture capture;
public:
	StreamImpl();
	virtual void jumpTo(unsigned long int);
	virtual void operator*(cv::Mat&) const;
	virtual void next(cv::Mat&);
	virtual bool end();
};

#endif // STREAMIMPL_H
