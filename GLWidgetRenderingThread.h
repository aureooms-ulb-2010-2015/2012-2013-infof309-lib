#ifndef GLWIDGETRENDERINGTHREAD_H
#define GLWIDGETRENDERINGTHREAD_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QQueue>
#include "OpenCV_SynchronizedGLWidget.h"

class GLWidgetRenderingThread : public QThread{

private:

	OpenCV_SynchronizedGLWidget* _widget;
	bool _pendingWork;
	QImage _waitingFrame;
	QWaitCondition _frameObserver;
	QMutex _frameLock;

public:
	GLWidgetRenderingThread(OpenCV_SynchronizedGLWidget* widget);
	void run();
	void delegateRenderImage(const QImage &frame);
	void delegateRefresh();
};


#endif // GLWIDGETRENDERINGTHREAD_H
