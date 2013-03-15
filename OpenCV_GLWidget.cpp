#include "OpenCV_GLWidget.h"
#include <iostream>

OpenCV_GLWidget::OpenCV_GLWidget(QWidget* parent, const QGLWidget* shareWidget, Qt::WindowFlags f) :
QGLWidget(parent, shareWidget, f){}

OpenCV_GLWidget::~OpenCV_GLWidget() {}

void OpenCV_GLWidget::renderImage(const QImage& frame){
	_GLFrame = QGLWidget::convertToGLFormat(frame);
	this->updateGL();
}

void OpenCV_GLWidget::resizeGL(int width, int height){
	// Setup our viewport to be the entire size of the window
	glViewport(0, 0, width, height);

	// Change to the projection matrix and set orthogonal projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

void OpenCV_GLWidget::paintGL() {
	glClear (GL_COLOR_BUFFER_BIT);
	glClearColor (0.0, 0.0, 0.0, 1.0);
	if (!_GLFrame.isNull()) {

		_GLFrame = _GLFrame.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

		glEnable(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, _GLFrame.width(), _GLFrame.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, _GLFrame.bits() );
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(0, _GLFrame.height());
		glTexCoord2f(0, 1); glVertex2f(0, 0);
		glTexCoord2f(1, 1); glVertex2f(_GLFrame.width(), 0);
		glTexCoord2f(1, 0); glVertex2f(_GLFrame.width(), _GLFrame.height());
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glFlush();

	}
}
