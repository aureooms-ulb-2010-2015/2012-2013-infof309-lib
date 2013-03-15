#ifndef VIDEOSTREAMINFO_H
#define VIDEOSTREAMINFO_H



class VideoStreamInfo{
private:
	bool _open;
	int _frameHeight;
	int _frameWidth;
	int _fps;
	int _numberOfFrames;
	int _actualFrameNumber;

public:
	VideoStreamInfo(bool open = false, int frameHeight = -1, int frameWidth = -1, int fps = -1, int numberOfFrames = -1,
					int actualFrameNumber = -1):_open(open),_frameHeight(frameHeight),
		_frameWidth(frameWidth),_fps(fps),_numberOfFrames(numberOfFrames),_actualFrameNumber(actualFrameNumber){

	}

	const bool& isOpen() const{return this->_open;}
	const int& getFps() const{return this->_fps;}
	const int& getNumberOfFrames() const{return this->_numberOfFrames;}
	const int& getActualFrameNumber() const{return this->_actualFrameNumber;}
	const int& getFrameHeight() const{return this->_frameHeight;}
	const int& getFrameWidth() const{return this->_frameWidth;}

};

#endif // VIDEOSTREAMINFO_H
