#include "SwaperWidget.h"

WidgetSwaper::WidgetSwaper(QWidget *parent, QWidget* widget1, QWidget* widget2) :
    QWidget(parent),_widget1(widget1),_widget2(widget2)
{
	this->_widget1->setParent(this);
	this->_widget2->setParent(this);
    this->_widget2->hide();
    this->_state = WidgetSwaper::first;
    this->setCursor(Qt::PointingHandCursor);
}

void WidgetSwaper::swap(){
    if(this->_state == WidgetSwaper::first){
        this->_widget1->hide();
        this->_widget2->show();
        this->_state = WidgetSwaper::second;
        emit clickedStateSecond();
    }
    else{
        this->_widget2->hide();
        this->_widget1->show();
        this->_state = WidgetSwaper::first;
        emit clickedStateFirst();
	}
}

void WidgetSwaper::setFirstState(){
	if(this->isSecondState()){
		this->_widget2->hide();
		this->_widget1->show();
		this->_state = WidgetSwaper::first;
	}
}

void WidgetSwaper::setSecondState(){
	if(this->isFirstState()){
		this->_widget1->hide();
		this->_widget2->show();
		this->_state = WidgetSwaper::second;
	}
}

bool WidgetSwaper::isFirstState(){
	return this->_state == WidgetSwaper::first;
}

bool WidgetSwaper::isSecondState(){
	return this->_state == WidgetSwaper::second;
}

void WidgetSwaper::mousePressEvent(QMouseEvent*){
    this->swap();
}
