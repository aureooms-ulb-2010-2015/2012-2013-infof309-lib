#include "OpenCV_SynchronizedGLWidget.h"
#include <iostream>

OpenCV_SynchronizedGLWidget::OpenCV_SynchronizedGLWidget(QWidget* parent, const QGLWidget* shareWidget, Qt::WindowFlags f) :
OpenCV_GLWidget(parent, shareWidget, f){}

OpenCV_SynchronizedGLWidget::~OpenCV_SynchronizedGLWidget() {}

void OpenCV_SynchronizedGLWidget::renderImage(const QImage& frame){
	this->_bufferLock.lock();
	this->_GLFrame = QGLWidget::convertToGLFormat(frame);
	this->_bufferLock.unlock();
	this->updateGL();
}

void OpenCV_SynchronizedGLWidget::resizeGL(int width, int height){
	this->_paintLock.lock();
	OpenCV_GLWidget::resizeGL(width,height);
	this->_paintLock.unlock();
}

void OpenCV_SynchronizedGLWidget::paintGL() {
	this->_paintLock.lock();
	glClear (GL_COLOR_BUFFER_BIT);
	glClearColor (0.0, 0.0, 0.0, 1.0);
	this->_bufferLock.lock();
	QImage current = this->_GLFrame;
	this->_bufferLock.unlock();
	if (!current.isNull()) {

		current = current.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		//glDrawPixels(current.width(),current.height(), GL_RGBA, GL_UNSIGNED_BYTE, current.bits());

		glEnable(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, current.width(), current.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, current.bits() );
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(0, current.height());
		glTexCoord2f(0, 1); glVertex2f(0, 0);
		glTexCoord2f(1, 1); glVertex2f(current.width(), 0);
		glTexCoord2f(1, 0); glVertex2f(current.width(), current.height());
		glEnd();
		glDisable(GL_TEXTURE_2D);


		glFlush();
/*
		QPainter p(this);

		//Set the painter to use a smooth scaling algorithm.
		p.setRenderHint(QPainter::SmoothPixmapTransform, 1);

		p.drawImage(this->rect(), current);*/

	}
	this->_paintLock.unlock();
}
