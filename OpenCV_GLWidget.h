#ifndef OPENCV_GLWIDGET_H
#define OPENCV_GLWIDGET_H

#include <QGLWidget>
#include <QImage>

class OpenCV_GLWidget: public QGLWidget {
public:
	OpenCV_GLWidget(QWidget * parent = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0);
	virtual ~OpenCV_GLWidget();
	virtual void renderImage(const QImage& frame);

protected:
	virtual void paintGL();
	virtual void resizeGL(int width, int height);
	QImage _GLFrame;
};

#endif // OPENCV_GLWIDGET_H_
