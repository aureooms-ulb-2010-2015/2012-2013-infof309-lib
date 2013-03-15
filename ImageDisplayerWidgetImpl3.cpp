#include "ImageDisplayerWidgetImpl3.h"
#include <iostream>
#include <QStyleOption>
#include <QPainter>

ImageDisplayerWidgetImpl3::ImageDisplayerWidgetImpl3(QWidget *parent) :
	ImageDisplayerWidget(parent),_frame(new OpenCV_SynchronizedGLWidget(this)),_renderingThread(this->_frame){

	this->setStyleSheet("QWidget{background-color:#000000}");
	this->setMinimumSize(640,480);
	this->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
	this->_renderingThread.start();
}

void ImageDisplayerWidgetImpl3::displayImage(QImage img){
	this->_height = img.height();
	this->_width = img.width();
	this->_renderingThread.delegateRenderImage(img);
}

void ImageDisplayerWidgetImpl3::setDimensions(int height, int width){
	float height_f = height, width_f = width;
	int frame_height = this->height(), frame_width = this->width();
	if( width_f/(float)frame_width >= height_f/(float)frame_height){
		int newHeight = height_f*(float)frame_width/width_f;
		int top = (frame_height-newHeight)/2;
		this->_frame->setGeometry(0,top,frame_width,newHeight);
	}
	else{
		int newWidth = width_f*(float)frame_height/height_f;
		int left = (frame_width-newWidth)/2;
		this->_frame->setGeometry(left,0,newWidth,frame_height);
	}
}


void ImageDisplayerWidgetImpl3::displayImageSLOT(QImage img){
	this->displayImage(img);
}

void ImageDisplayerWidgetImpl3::paintEvent(QPaintEvent*) {
	QStyleOption o;
	o.initFrom(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}

void ImageDisplayerWidgetImpl3::reset(){
	QImage blackPixel(1,1,QImage::Format_RGB888);
	blackPixel.fill(Qt::black);
	this->_renderingThread.delegateRenderImage(blackPixel);
}

void ImageDisplayerWidgetImpl3::resizeEvent(QResizeEvent *){
	this->setDimensions(this->_height, this->_width);
	this->_renderingThread.delegateRefresh();
}
