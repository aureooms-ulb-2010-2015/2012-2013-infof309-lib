#include "TimeDisplayerWidget.h"
#include <QTime>
TimeDisplayerWidget::TimeDisplayerWidget(QWidget *parent) :
    QWidget(parent){
    this->_layout = new QHBoxLayout();
    this->_elapsed = new QLabel();
	this->_elapsed->setText("00:00");
	this->_separator = new QLabel();
	this->_separator->setText("/");
	this->_total = new QLabel();
	this->_total->setText("00:00");
	this->_layout->addWidget(this->_elapsed);
	this->_layout->addWidget(this->_separator);
	this->_layout->addWidget(this->_total);
    this->setLayout(this->_layout);
	this->_layout->setSpacing(0);
	this->_layout->setMargin(0);
	this->_layout->setContentsMargins(0,0,0,0);

	QPalette color;
	color.setColor(QPalette::WindowText, QColor(200,20,20));
	this->_elapsed->setPalette(color);

	this->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
}


void TimeDisplayerWidget::setElapsedTime(int s){
	QTime time(0,0,0,0);
    time = time.addSecs(s);
	QString format = (time.hour()>0)? "h:mm:ss" : "mm:ss";
    this->_elapsed->setText(time.toString(format));
}

void TimeDisplayerWidget::setTotalTime(int s){
	QTime time(0,0,0,0);
    time = time.addSecs(s);
	QString format = (time.hour()>0)? "h:mm:ss" : "mm:ss";
    this->_total->setText(time.toString(format));
}
