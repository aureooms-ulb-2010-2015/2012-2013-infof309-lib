#include "TimeControlWidget.h"
#include <QFile>
#include <QMouseEvent>
#include <iostream>

TimeControlWidget::TimeControlWidget(QWidget *parent) :
	QSlider(parent){
	this->setOrientation(Qt::Horizontal);
	QFile file(":/qss/timeControlSlider.qss");
	file.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(file.readAll());
	this->setStyleSheet(styleSheet);
	file.close();
}


void TimeControlWidget::mousePressEvent(QMouseEvent *e){
	if (e->button() == Qt::LeftButton){
		this->_lastX = e->x();
		int x = (float)this->value()/(float)this->maximum()*this->width();
		if (this->rect().contains(e->pos()) && (x > e->x()+8 || x < e->x()-8)){
			e->accept();
		}
		else{
			QSlider::mousePressEvent(e);
		}
	}
}

void TimeControlWidget::mouseReleaseEvent(QMouseEvent *e){
	if (e->button() == Qt::LeftButton){
		int x = (float)this->value()/(float)this->maximum()*this->width();
		if (this->rect().contains(e->pos()) && this->_lastX == e->x() && (x > e->x()+8 || x < e->x()-8)){
			int newVal = (float)e->x()/(float)this->width()*this->maximum();
			this->setValue(newVal);
			e->accept();
			emit sliderMoved(newVal);
		}
		else{
			QSlider::mouseReleaseEvent(e);
		}
	}
}
