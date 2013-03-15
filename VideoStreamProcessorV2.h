#ifndef VIDEOSTREAMPROCESSORV2_H
#define VIDEOSTREAMPROCESSORV2_H

#include "Stream.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QThread>
#include <QQueue>
#include <QWaitCondition>
#include <QMutex>
#include <iostream>

#include "VideoStreamInfo.h"
#include "FrameProcessor.h"



class VideoStreamProcessorV2 : public Stream<cv::Mat> {

private:

	Stream<cv::Mat>* _source;
	FrameProcessor* _processor;
	QMutex _lock;

public:


	VideoStreamProcessorV2(Stream<cv::Mat>* source, FrameProcessor* processor);
	virtual void jumpTo(unsigned long int pos);
	virtual int next(cv::Mat& out);
	virtual void reset(Stream<cv::Mat>* source, FrameProcessor* processor);
};

#endif // VIDEOSTREAMPROCESSORV2_H
