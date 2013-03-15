#ifndef VIDEOFILEREADER_H
#define VIDEOFILEREADER_H

#include "Stream.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QMutex>
#include <iostream>

#include "VideoStreamInfo.h"



class VideoStreamReader : public Stream<cv::Mat> {

private:

	cv::VideoCapture _source;
	QMutex _lock;
	int _position;

public:


	VideoStreamReader():_position(0){}
	virtual ~VideoStreamReader(){}

    /*
     * Moves lecture head to pos.
     */
    virtual void jumpTo(unsigned long int pos){
		this->_lock.lock();
		this->_source.set(CV_CAP_PROP_POS_FRAMES, pos);
		this->_position = pos;
		this->_lock.unlock();
    }


    /*
     * Returns the head frame and moves to next.
     */
	virtual int next(cv::Mat& out){
		int n;
		this->_lock.lock();
		bool data = this->_source.read(out);
		if(!data){
			n = -1;
		}
		else{
			n = this->_position++;
		}
		this->_lock.unlock();
		return n;
	}

	virtual void log(){
		std::cout << "POS_MSEC " << this->_source.get(CV_CAP_PROP_POS_MSEC) << std::endl;
		std::cout << "POS_FRAMES " << this->_source.get(CV_CAP_PROP_POS_FRAMES) << std::endl;
		std::cout << "POS_AVI_RATIO " << this->_source.get(CV_CAP_PROP_POS_AVI_RATIO) << std::endl;
		std::cout << "FRAME_WIDTH " << this->_source.get(CV_CAP_PROP_FRAME_WIDTH) << std::endl;
		std::cout << "FRAME_HEIGHT " << this->_source.get(CV_CAP_PROP_FRAME_HEIGHT) << std::endl;
		std::cout << "FPS " << this->_source.get(CV_CAP_PROP_FPS) << std::endl;
		std::cout << "FOURCC " << this->_source.get(CV_CAP_PROP_FOURCC) << std::endl;
		std::cout << "FRAME_COUNT " << this->_source.get(CV_CAP_PROP_FRAME_COUNT) << std::endl;
		std::cout << "FORMAT " << this->_source.get(CV_CAP_PROP_FORMAT) << std::endl;
		std::cout << "MODE " << this->_source.get(CV_CAP_PROP_MODE) << std::endl;
		std::cout << "BRIGHTNESS " << this->_source.get(CV_CAP_PROP_BRIGHTNESS) << std::endl;
		std::cout << "CONTRAST " << this->_source.get(CV_CAP_PROP_CONTRAST) << std::endl;
		std::cout << "SATURATION " << this->_source.get(CV_CAP_PROP_SATURATION) << std::endl;
		std::cout << "HUE " << this->_source.get(CV_CAP_PROP_HUE) << std::endl;
		std::cout << "GAIN " << this->_source.get(CV_CAP_PROP_GAIN) << std::endl;
		std::cout << "EXPOSURE " << this->_source.get(CV_CAP_PROP_EXPOSURE) << std::endl;
		std::cout << "CONVERT_RGB " << this->_source.get(CV_CAP_PROP_CONVERT_RGB) << std::endl;
		std::cout << "RECTIFICATION " << this->_source.get(CV_CAP_PROP_RECTIFICATION) << std::endl;
	}

	virtual VideoStreamInfo getSourceInfo(){
		bool open;
		int frameHeight = -1, frameWidth = -1, fps = -1, numberOfFrames = -1, actualFrameNumber = -1;
		this->_lock.lock();
		open = this->_source.isOpened();
		if (open){
			frameHeight = this->_source.get(CV_CAP_PROP_FRAME_HEIGHT);
			frameWidth = this->_source.get(CV_CAP_PROP_FRAME_WIDTH);
			fps = this->_source.get(CV_CAP_PROP_FPS);
			numberOfFrames = this->_source.get(CV_CAP_PROP_FRAME_COUNT);
			actualFrameNumber = this->_source.get(CV_CAP_PROP_POS_FRAMES);
		}
		this->_lock.unlock();
		return VideoStreamInfo(open, frameHeight, frameWidth, fps, numberOfFrames, actualFrameNumber);
	}

	void open(QString fileName){
		std::cout << fileName.toStdString() << std::endl;
		this->_lock.lock();
        if(this->_source.isOpened()){
            this->_source.release();
        }
        this->_source.open(fileName.toStdString());

		this->_position = 0;
		this->_lock.unlock();
	}

	void open(int device){
		std::cout << "device# " << device << std::endl;
		this->_lock.lock();
		if(this->_source.isOpened()){
			this->_source.release();
		}
		this->_source.open(device);

		this->_position = 0;
		this->_lock.unlock();

	}
};

#endif // VIDEOFILEREADER_H
