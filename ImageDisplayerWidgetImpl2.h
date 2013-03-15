#ifndef IMAGEDISPLAYERWIDGETIMPL2_H
#define IMAGEDISPLAYERWIDGETIMPL2_H

#include "ImageDisplayerWidget.h"
#include "OpenCV_GLWidget.h"

class ImageDisplayerWidgetImpl2 : public ImageDisplayerWidget{
	Q_OBJECT

private:
	OpenCV_GLWidget* _frame;

public:
	explicit ImageDisplayerWidgetImpl2(QWidget *parent = 0);
	virtual void displayImage(QImage);
	virtual void setDimensions(int height, int width);
	virtual void paintEvent(QPaintEvent *pe);
	virtual void reset();

public slots:
	virtual void displayImageSLOT(QImage);

};



#endif // IMAGEDISPLAYERWIDGETIMPL2_H
