#include "BasicWindow.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QDesktopWidget>

void BasicWindow::centerWindow(){
	this->show();
	QRect r(this->frameGeometry());
	r.moveCenter(QApplication::desktop()->availableGeometry().center());
	this->move(r.topLeft());
}

BasicWindow::BasicWindow(QWidget *parent) :
	QWidget(parent){

	this->_wraper->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	this->_upperBar->setFixedWidth(640);
	this->_upperBar->setFixedHeight(30);
	this->_videoControls->setFixedWidth(640);
	//--------------------------------------------------------------------
	this->_layout->addWidget(this->_upperBar,0,0,1,1);
	this->_layout->addWidget(this->_videoFrame,1,0,1,1);
	this->_layout->addWidget(this->_videoControls,2,0,1,1);
	//--------------------------------------------------------------------


	QObject::connect(this->_inputChoice, SIGNAL(currentSourceChanged(int)), this, SLOT(requestChangeSourceSLOT(int)));
	QObject::connect(this->_inputChoice, SIGNAL(currentSourceChanged(QString)), this, SLOT(requestChangeSourceSLOT(QString)));

	QObject::connect(this->_displayer, SIGNAL(displaySIGNAL(QImage)), this->_videoFrame, SLOT(displayImageSLOT(QImage)));
	QObject::connect(this->_displayer, SIGNAL(posSIGNAL(int)), this, SLOT(posChangedSLOT(int)));


	QObject::connect(this->_videoControls, SIGNAL(pause()), this->_displayer, SLOT(pause()));
	QObject::connect(this->_videoControls, SIGNAL(play()), this->_displayer, SLOT(play()));
	QObject::connect(this->_videoControls, SIGNAL(timeChanged(int)), this, SLOT(timeChangedSLOT(int)));


	this->setLayout(this->_layout);
	this->_streamProcessor->start();
	this->_displayer->start();
	this->centerWindow();
}

BasicWindow::~BasicWindow(){
	delete this->_streamReader;
	delete this->_streamProcessor;
	delete this->_frameProcessor;
	delete this->_displayer;
}

void BasicWindow::keyPressEvent(QKeyEvent* event){

	if(event->key() == Qt::Key_F11){
		if (!this->isFullScreen()){
			return this->requestEnterFullScreenSLOT();
		}
		else{
			return this->requestExitFullScreenSLOT();
		}
	}
	else if(event->key() == Qt::Key_F4 && QApplication::keyboardModifiers() == Qt::AltModifier){
		return QCoreApplication::quit();
	}

	else if(event->key() == Qt::Key_F5){
		return reloadStream();
	}

	else if(event->key() == Qt::Key_F1){
		return this->playPauseToggle();
	}

	else if(event->key() == Qt::Key_F2){
		return this->_inputChoice->setCurrentIndex(1);
	}
	else if(event->key() == Qt::Key_F3){
		return this->_inputChoice->setCurrentIndex(2);
	}
	else if(event->key() == Qt::Key_F4 && this->_inputChoice->count() >= 4){
		return this->_inputChoice->setCurrentIndex(3);
	}

	return QWidget::keyPressEvent(event);
}

void BasicWindow::requestChangeSourceSLOT(int device){
	lastDevice = device;
	whichLast = 0;
	this->_displayer->pause();
	this->_streamProcessor->stop();
	this->_streamReader->open(device);
	this->sourceReady();
}

void BasicWindow::requestChangeSourceSLOT(QString filename){
	lastPath = filename;
	whichLast = 1;
	this->_displayer->pause();
	this->_streamProcessor->stop();
	this->_streamReader->open(filename);
	this->sourceReady();
}

void BasicWindow::sourceReady(){
	FrameProcessor* temp = this->_frameProcessor;
	this->_frameProcessor = this->generateProcessor();
	delete temp;
	this->_streamProcessor->reset(this->_streamReader, this->_frameProcessor);
	this->_streamInfo = this->_streamReader->getSourceInfo();
	int totalTime = 0, elapsedTime = 0;
	this->_displayer->setFrameLength((this->_streamInfo.getFps() <= 0)? 40 : 1000/this->_streamInfo.getFps());

	if(this->_streamInfo.isOpen()){
		if(this->_streamInfo.getFps() > 0 && this->_streamInfo.getNumberOfFrames() > 0 && this->_streamInfo.getActualFrameNumber() >= 0){
			totalTime = this->_streamInfo.getNumberOfFrames()/this->_streamInfo.getFps();
			elapsedTime = this->_streamInfo.getActualFrameNumber()/this->_streamInfo.getFps();
		}
	}
	this->_videoControls->setTotalTime(totalTime);
	this->_videoControls->setElapsedTime(elapsedTime);
	this->_videoFrame->reset();
	this->_videoFrame->setDimensions(this->_streamInfo.getFrameHeight(), this->_streamInfo.getFrameWidth());
	this->_displayer->play();
	this->_videoControls->setPlaying();
}

void BasicWindow::reloadStream(){
	if(whichLast == -1) return;
	if(whichLast == 0){
		this->requestChangeSourceSLOT(lastDevice);
	}
	else{
		this->requestChangeSourceSLOT(lastPath);
	}
}

void BasicWindow::playPauseToggle(){
	bool wasPlaying = this->_videoControls->isPlaying();
	if(wasPlaying){
		this->_displayer->pause();
		this->_videoControls->setPaused();
	}
	else{
		this->_displayer->play();
		this->_videoControls->setPlaying();
	}
}


void BasicWindow::posChangedSLOT(int pos){
	int elapsedTime = 0;
	if(this->_streamInfo.getFps() > 0 && this->_streamInfo.getNumberOfFrames() > 0 && pos >= 0){
		elapsedTime = pos/this->_streamInfo.getFps();
	}
	this->_videoControls->setElapsedTime(elapsedTime);
	if(pos == this->_streamInfo.getNumberOfFrames() - 1){
		reloadStream();
	}
}

void BasicWindow::timeChangedSLOT(int time){
	this->_streamProcessor->jumpTo(time*this->_streamInfo.getFps());
}

void BasicWindow::requestEnterFullScreenSLOT(){
	bool wasPlaying = this->_videoControls->isPlaying();
	if(wasPlaying){
		this->_displayer->pause();
	}
	this->_streamProcessor->stop();
	this->_videoFrame->reset();

	this->_upperBar->hide();
	this->_videoControls->hide();
	this->showFullScreen();
	this->_videoFrame->setMinimumSize(0,0);
	this->_videoFrame->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
	this->_layout->setContentsMargins(0,0,0,0);

	this->_streamProcessor->reset(this->_streamReader, this->_frameProcessor);
	if(wasPlaying){
		this->_displayer->play();
		this->_videoControls->setPlaying();
	}
}

void BasicWindow::requestExitFullScreenSLOT(){
	bool wasPlaying = this->_videoControls->isPlaying();
	if(wasPlaying){
		this->_displayer->pause();
	}
	this->_streamProcessor->stop();
	this->_videoFrame->reset();

	this->showNormal();
	this->_upperBar->show();
	this->_videoControls->show();
	this->_videoFrame->setFixedSize(640,480);
	this->_layout->setContentsMargins(10,10,10,10);

	this->_streamProcessor->reset(this->_streamReader, this->_frameProcessor);
	if(wasPlaying){
		this->_displayer->play();
		this->_videoControls->setPlaying();
	}
}
