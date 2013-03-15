#ifndef OPENCV_SYNCHRONIZEDGLWIDGET_H
#define OPENCV_SYNCHRONIZEDGLWIDGET_H

#include "OpenCV_GLWidget.h"
#include <QMutex>

class OpenCV_SynchronizedGLWidget : public OpenCV_GLWidget {
public:
	OpenCV_SynchronizedGLWidget(QWidget * parent = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0);
	virtual ~OpenCV_SynchronizedGLWidget();
	virtual void renderImage(const QImage& frame);

protected:
	virtual void paintGL();
	virtual void resizeGL(int width, int height);

private:
	QMutex _paintLock;
	QMutex _bufferLock;
};

#endif // OPENCV_SYNCHRONIZEDGLWIDGET_H
