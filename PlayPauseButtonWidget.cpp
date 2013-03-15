#include "PlayPauseButtonWidget.h"

PlayPauseButtonWidget::PlayPauseButtonWidget(QWidget *parent):QWidget(parent){
	QLabel *label1 = new QLabel();
	label1->setPixmap(QPixmap(":pause_icon"));
	QLabel *label2 = new QLabel();
	label2->setPixmap(QPixmap(":play_icon"));
	label1->setFixedSize(32,32);
	label2->setFixedSize(32,32);
	this->setFixedSize(32,32);
	this->_button = new WidgetSwaper(this, label1, label2);
	this->_button->setFixedSize(32,32);
	this->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
	QObject::connect(this->_button, SIGNAL(clickedStateSecond()), this, SIGNAL(pause()));
	QObject::connect(this->_button, SIGNAL(clickedStateFirst()), this, SIGNAL(play()));
}

void PlayPauseButtonWidget::setPause(){
	this->_button->setSecondState();
}

void PlayPauseButtonWidget::setPlay(){
	this->_button->setFirstState();
}

bool PlayPauseButtonWidget::isPause(){
	return this->_button->isSecondState();
}

bool PlayPauseButtonWidget::isPlay(){
	return this->_button->isFirstState();
}
