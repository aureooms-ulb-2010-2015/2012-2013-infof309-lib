#include "VideoStreamProcessor.h"


void VideoStreamProcessor::run(){
	while(true){
		cv::Mat frame;
		this->_lock.lock();
		while(this->_output.size() >= this->_maxBufferSize){
			this->_queueObserver.wait(&this->_lock);
		}

		//retrieve
		this->_position = this->_source->next(frame);
		while(this->_position == -1){
			this->_sourceObserver.wait(&this->_lock);
			this->_position = this->_source->next(frame);
		}

		//process
		cv::Mat out;
		this->_processor->process(frame,out);

		//enqueue
		this->_output.enqueue(out);
		this->_queueObserver.wakeOne();
		this->_lock.unlock();
		QThread::yieldCurrentThread();
	}
}

VideoStreamProcessor::VideoStreamProcessor(Stream<cv::Mat>* source, FrameProcessor* processor, size_t minBufferSize,
					 size_t maxBufferSize):_source(source),_processor(processor),_minBufferSize(minBufferSize),
	_maxBufferSize(maxBufferSize),_position(0){}

void VideoStreamProcessor::jumpTo(unsigned long int pos){
	this->_lock.lock();
	this->_source->jumpTo(pos);
	this->_output.clear();
	this->_position = pos;
	this->_queueObserver.wakeOne();
	this->_sourceObserver.wakeOne();
	this->_lock.unlock();
}

int VideoStreamProcessor::next(cv::Mat& out){
	this->_lock.lock();
	while(this->_output.empty()){
		this->_queueObserver.wait(&this->_lock);
	}
	out = this->_output.dequeue();
	if(this->_output.size() < this->_minBufferSize){
		this->_queueObserver.wakeOne();
	}
	int n = this->_position++;
	this->_lock.unlock();
	return n;
}
void VideoStreamProcessor::stop(){
	this->_lock.lock();
}

void VideoStreamProcessor::reset(Stream<cv::Mat>* source, FrameProcessor* processor,
								 size_t minBufferSize,
								 size_t maxBufferSize){
	this->_output.clear();
	this->_position = 0;
	this->_source = source;
	this->_processor = processor;
	this->_minBufferSize = minBufferSize;
	this->_maxBufferSize = maxBufferSize;
	this->_queueObserver.wakeAll();
	this->_sourceObserver.wakeOne();
	this->_lock.unlock();
}
