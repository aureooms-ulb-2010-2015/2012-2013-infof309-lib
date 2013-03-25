#include "CustomComboBox.h"
#include <QFile>
#include <QApplication>


CustomComboBox::CustomComboBox(QWidget *parent) :
    QComboBox(parent){
    QFile temp(":/qss/QComboBox.qss");
    temp.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(temp.readAll());
    this->setStyleSheet(styleSheet);
    temp.close();
}

void CustomComboBox::keyPressEvent(QKeyEvent *event){
	if(QApplication::keyboardModifiers() & Qt::ControlModifier){
		return event->ignore();
	}
	return QComboBox::keyPressEvent(event);
}
