#ifndef STREAMDISPLAYER_H
#define STREAMDISPLAYER_H

#include <QThread>
#include <QWaitCondition>
#include "Stream.h"

#define FRAME_LENGTH 0

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QImage>
#include <QMutex>
#include <iostream>
#include "Mat2QImage.h"

class StreamDisplayer: public QThread{
	Q_OBJECT

private:

	Stream<cv::Mat>* const _source;
	unsigned long int _frameLength;

	bool _running;

	QWaitCondition _loopObserver;
	QMutex _loopLock;
	QMutex _frameLengthLock;


protected:

	virtual void run(){
		while(true){
			cv::Mat frame;
			this->_loopLock.lock();
			while(!this->_running){
				this->_loopObserver.wait(&this->_loopLock);
			}
			this->_loopLock.unlock();
			int pos = this->_source->next(frame);
			this->display(pos, frame);
			this->delay();
		}
    }

public:


	StreamDisplayer(Stream<cv::Mat>* source, unsigned long int frameLength = FRAME_LENGTH):_source(source),_frameLength(frameLength),_running(true){

	}

    virtual void delay(){
		QThread::msleep(this->getFrameLength());
	}

	virtual void display(int pos, const cv::Mat& frame){
		emit displaySIGNAL(Mat2QImage::exec(frame));
		emit posSIGNAL(pos);
	}

	virtual void setFrameLength(unsigned long int frameLength){
		this->_frameLengthLock.lock();
		this->_frameLength = frameLength;
		this->_frameLengthLock.unlock();
	}

	virtual unsigned long int getFrameLength(){
		unsigned long int frameLength;
		this->_frameLengthLock.lock();
		frameLength = this->_frameLength;
		this->_frameLengthLock.unlock();
		return frameLength;
	}

public slots:
	void pause(){
		this->_loopLock.lock();
		this->_running = false;
		this->_loopLock.unlock();
	}

	void play(){
		this->_loopLock.lock();
		this->_running = true;
		this->_loopObserver.wakeOne();
		this->_loopLock.unlock();
	}


signals:
	void displaySIGNAL(QImage);
	void posSIGNAL(int);

};


#endif // STREAMDISPLAYER_H
