#include "VideoPlayControlsWidget.h"
#include <QHBoxLayout>
#include <iostream>

VideoPlayControlsWidget::VideoPlayControlsWidget(QWidget *parent) :
	QWidget(parent), _totalTime(0), _elapsedTime(0){

	this->_playPauseButton = new PlayPauseButtonWidget();
	this->_timeControl = new TimeControlWidget();
	this->_timeDisplayer = new TimeDisplayerWidget();

	this->_layout = new QHBoxLayout(this);
	this->_layout->addWidget(this->_playPauseButton);
	this->_layout->addWidget(this->_timeControl);
	this->_layout->addWidget(this->_timeDisplayer);
	this->_layout->setSpacing(10);
	this->_layout->setMargin(0);
	this->_layout->setContentsMargins(10,10,10,10);


	this->setFixedHeight(60);

    this->setTotalTime(0);
	this->setElapsedTime(0);


	QObject::connect(this->_playPauseButton, SIGNAL(pause()), this, SIGNAL(pause()));
	QObject::connect(this->_playPauseButton, SIGNAL(play()), this, SIGNAL(play()));
	QObject::connect(this->_timeControl, SIGNAL(valueChanged(int)), this, SLOT(sliderMovedSLOT(int)));
	QObject::connect(this->_timeControl,SIGNAL(sliderPressed()), this, SLOT(seekStartSLOT()));
	QObject::connect(this->_timeControl,SIGNAL(sliderReleased()), this, SLOT(seekEndSLOT()));
}

void VideoPlayControlsWidget::setElapsedTime(int s){
    if(this->_elapsedTime != s){
        this->_elapsedTime = s;
        this->_timeDisplayer->setElapsedTime(s);
        this->_timeControl->setSliderPosition(s);
    }
}

void VideoPlayControlsWidget::setTotalTime(int s){
	this->_totalTime = s;
	this->_timeDisplayer->setTotalTime(s);
	this->_timeControl->setRange(0,s);
}

bool VideoPlayControlsWidget::isPaused(){
	return this->_playPauseButton->isPause();
}

bool VideoPlayControlsWidget::isPlaying(){
	return this->_playPauseButton->isPlay();
}

void VideoPlayControlsWidget::setElapsedTimeSLOT(int s){
	this->setElapsedTime(s);
}

void VideoPlayControlsWidget::setTotalTimeSLOT(int s){
	this->setTotalTime(s);
}


void VideoPlayControlsWidget::sliderMovedSLOT(int s){
    if(this->_elapsedTime != s){
        this->_elapsedTime = s;
        this->_timeDisplayer->setElapsedTime(s);
        emit timeChanged(s);
	}
}

void VideoPlayControlsWidget::seekStartSLOT(){
	if(this->isPlaying()){
		emit pause();
	}
}

void VideoPlayControlsWidget::seekEndSLOT(){
	if(this->isPlaying()){
		emit play();
	}
}

void VideoPlayControlsWidget::setPlaying(){
	this->_playPauseButton->setPlay();
}

void VideoPlayControlsWidget::setPaused(){
	this->_playPauseButton->setPause();
}
