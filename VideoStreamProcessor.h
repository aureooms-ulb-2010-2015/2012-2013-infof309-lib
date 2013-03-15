#ifndef VIDEOSTREAMPROCESSOR_H
#define VIDEOSTREAMPROCESSOR_H

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

#define STREAMPROCESSOR_MAX_BUFFER_SIZE 1
#define STREAMPROCESSOR_MIN_BUFFER_SIZE 1



class VideoStreamProcessor : public QThread, public Stream<cv::Mat> {
	Q_OBJECT

private:

	Stream<cv::Mat>* _source;
	FrameProcessor* _processor;
	QQueue<cv::Mat> _output;
	int _minBufferSize;
	int _maxBufferSize;
	QWaitCondition _queueObserver;
	QWaitCondition _sourceObserver;
	QMutex _lock;
	int _position;


protected:

	virtual void run();

public:


	VideoStreamProcessor(Stream<cv::Mat>* source, FrameProcessor* processor,
						 size_t minBufferSize = STREAMPROCESSOR_MIN_BUFFER_SIZE,
						 size_t maxBufferSize = STREAMPROCESSOR_MAX_BUFFER_SIZE);
	virtual void jumpTo(unsigned long int pos);
	virtual int next(cv::Mat& out);
	virtual void stop();
	virtual void reset(Stream<cv::Mat>* source, FrameProcessor* processor,
					   size_t minBufferSize = STREAMPROCESSOR_MIN_BUFFER_SIZE,
					   size_t maxBufferSize = STREAMPROCESSOR_MAX_BUFFER_SIZE);
};

#endif // VIDEOSTREAMPROCESSOR_H
