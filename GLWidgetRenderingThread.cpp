#include "GLWidgetRenderingThread.h"
#include <iostream>

GLWidgetRenderingThread::GLWidgetRenderingThread(OpenCV_SynchronizedGLWidget *widget): _widget(widget),_pendingWork(false){}

void GLWidgetRenderingThread::run(){

	this->_widget->makeCurrent();
	while (true) {
		QImage temp;
		this->_frameLock.lock();
		while(!this->_pendingWork){
			this->_frameObserver.wait(&this->_frameLock);
		}
		temp = this->_waitingFrame;
		this->_pendingWork = false;
		this->_frameLock.unlock();
		this->_widget->renderImage(temp);
		this->_widget->swapBuffers();
	}
}

void GLWidgetRenderingThread::delegateRenderImage(const QImage& frame){
	this->_frameLock.lock();
	this->_waitingFrame = frame.copy();
	this->_pendingWork = true;
	this->_frameObserver.wakeOne();
	this->_frameLock.unlock();
}

void GLWidgetRenderingThread::delegateRefresh(){
	this->_frameLock.lock();
	this->_pendingWork = true;
	this->_frameObserver.wakeOne();
	this->_frameLock.unlock();
}
