#ifndef IMAGEDISPLAYERWIDGET_H
#define IMAGEDISPLAYERWIDGET_H

#include <QWidget>

class ImageDisplayerWidget : public QWidget{
	Q_OBJECT

public:
	explicit ImageDisplayerWidget(QWidget *parent = 0):QWidget(parent){}
	virtual void displayImage(QImage) = 0;
	virtual void setDimensions(int height, int width) = 0;
	virtual void reset() = 0;

public slots:
	virtual void displayImageSLOT(QImage) = 0;

};

#endif // IMAGEDISPLAYERWIDGET_H
