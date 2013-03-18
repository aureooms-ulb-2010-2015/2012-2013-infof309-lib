#include "CustomButton.h"

#include <QFile>

CustomButton::CustomButton(QWidget *parent) : QPushButton(parent){
	QFile file(":/qss/CustomButton.qss");
	file.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(file.readAll());
	this->setStyleSheet(styleSheet);
	file.close();
	this->setCursor(Qt::PointingHandCursor);
}

CustomButton::CustomButton(const QString &text, QWidget *parent) : QPushButton(text, parent){
	QFile file(":/qss/CustomButton.qss");
	file.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(file.readAll());
	this->setStyleSheet(styleSheet);
	file.close();
	this->setCursor(Qt::PointingHandCursor);
}
