#ifndef TIMECONTROLWIDGET_H
#define TIMECONTROLWIDGET_H

#include <QSlider>

class TimeControlWidget : public QSlider{
	Q_OBJECT
private:
	int _lastX;

public:
	explicit TimeControlWidget(QWidget *parent = 0);

	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
signals:
	
public slots:
	
};

#endif // TIMECONTROLWIDGET_H
