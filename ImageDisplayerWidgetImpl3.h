#ifndef IMAGEDISPLAYERWIDGETIMPL3_H
#define IMAGEDISPLAYERWIDGETIMPL3_H

#include "ImageDisplayerWidget.h"
#include "OpenCV_SynchronizedGLWidget.h"
#include "GLWidgetRenderingThread.h"

class ImageDisplayerWidgetImpl3 : public ImageDisplayerWidget{
	Q_OBJECT

private:
	OpenCV_SynchronizedGLWidget* _frame;
	GLWidgetRenderingThread _renderingThread;
	int _height;
	int _width;

public:
	explicit ImageDisplayerWidgetImpl3(QWidget *parent = 0);
	virtual void displayImage(QImage);
	virtual void setDimensions(int height, int width);
	virtual void paintEvent(QPaintEvent *pe);
	virtual void reset();

	void resizeEvent(QResizeEvent *);
public slots:
	virtual void displayImageSLOT(QImage);

};



#endif // IMAGEDISPLAYERWIDGETIMPL3_H
