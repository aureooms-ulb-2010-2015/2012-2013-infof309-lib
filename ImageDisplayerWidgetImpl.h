#ifndef IMAGEDISPLAYERWIDGETIMPL_H
#define IMAGEDISPLAYERWIDGETIMPL_H

#include "ImageDisplayerWidget.h"
#include <QLabel>

class ImageDisplayerWidgetImpl : public ImageDisplayerWidget{
    Q_OBJECT

private:
	QLabel* _label;
	QImage _img;

public:
	explicit ImageDisplayerWidgetImpl(QWidget *parent = 0);
	virtual void displayImage(QImage);
	virtual void setDimensions(int height, int width);
	virtual void paintEvent(QPaintEvent *pe);
	virtual void reset();
	virtual void resizeEvent(QResizeEvent *);

public slots:
	virtual void displayImageSLOT(QImage);
    
};

#endif // IMAGEDISPLAYERWIDGETIMPL_H
