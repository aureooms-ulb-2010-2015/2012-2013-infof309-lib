#include "VideoStreamProcessorV2.h"


VideoStreamProcessorV2::VideoStreamProcessorV2(Stream<cv::Mat>* source, FrameProcessor* processor)
	:_source(source),_processor(processor){}

void VideoStreamProcessorV2::jumpTo(unsigned long int pos){
	this->_lock.lock();
	this->_source->jumpTo(pos);
	this->_lock.unlock();
}

int VideoStreamProcessorV2::next(cv::Mat& out){
	int n;
	cv::Mat frame;
	this->_lock.lock();
	n = this->_source->next(frame);
	if(n != -1){
		this->_processor->process(frame,out);
	}
	this->_lock.unlock();
	return n;
}

void VideoStreamProcessorV2::reset(Stream<cv::Mat>* source, FrameProcessor* processor){
	this->_lock.lock();
	this->_source = source;
	this->_processor = processor;
	this->_lock.unlock();
}
