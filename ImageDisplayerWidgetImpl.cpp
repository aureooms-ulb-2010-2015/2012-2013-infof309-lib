#include "ImageDisplayerWidgetImpl.h"
#include <iostream>
#include <QStyleOption>
#include <QPainter>

ImageDisplayerWidgetImpl::ImageDisplayerWidgetImpl(QWidget *parent) :
	ImageDisplayerWidget(parent),_img(1,1,QImage::Format_RGB888){
	this->_img.fill(Qt::black);
	this->_label = new QLabel(this);
	this->setStyleSheet("QWidget{background-color:#000000}");
	this->setFixedSize(640,480);
}

void ImageDisplayerWidgetImpl::displayImage(QImage img){
	this->_img = img;
	this->_label->setPixmap(QPixmap::fromImage(this->_img.scaled(this->_label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
}

void ImageDisplayerWidgetImpl::setDimensions(int height, int width){
	float height_f = height, width_f = width;
	int frame_height = this->height(), frame_width = this->width();
	if( width_f/(float)frame_width >= height_f/(float)frame_height){
		int newHeight = height_f*(float)frame_width/width_f;
		int top = (frame_height-newHeight)/2;
		this->_label->setGeometry(0,top,frame_width,newHeight);
	}
	else{
		int newWidth = width_f*(float)frame_height/height_f;
		int left = (frame_width-newWidth)/2;
		this->_label->setGeometry(left,0,newWidth,frame_height);
	}
}

void ImageDisplayerWidgetImpl::displayImageSLOT(QImage img){
	this->displayImage(img);
}

void ImageDisplayerWidgetImpl::paintEvent(QPaintEvent*) {
	QStyleOption o;
	o.initFrom(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}
void ImageDisplayerWidgetImpl::reset(){
	QImage blackPixel(1,1,QImage::Format_RGB888);
	blackPixel.fill(Qt::black);
	this->_label->setPixmap(QPixmap::fromImage(blackPixel.scaled(this->_label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
}

void ImageDisplayerWidgetImpl::resizeEvent(QResizeEvent *){
	this->setDimensions(this->_img.height(), this->_img.width());
	this->_label->setPixmap(QPixmap::fromImage(this->_img.scaled(this->_label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
}
