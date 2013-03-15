#ifndef VIDEOPLAYCONTROLSWIDGET_H
#define VIDEOPLAYCONTROLSWIDGET_H

#include <QWidget>
#include "PlayPauseButtonWidget.h"
#include "TimeControlWidget.h"
#include "TimeDisplayerWidget.h"
#include <QLayout>

class VideoPlayControlsWidget : public QWidget
{
	Q_OBJECT
private:

	QLayout* _layout;

	PlayPauseButtonWidget* _playPauseButton;
	TimeControlWidget* _timeControl;
	TimeDisplayerWidget* _timeDisplayer;

	int _totalTime;
	int _elapsedTime;

public:
	explicit VideoPlayControlsWidget(QWidget *parent = 0);

	void setElapsedTime(int);
	void setTotalTime(int);
	bool isPaused();
	bool isPlaying();
	void setPlaying();
	void setPaused();
	
signals:

	void pause();
	void play();
	void timeChanged(int);

protected slots:

	void setElapsedTimeSLOT(int);
	void setTotalTimeSLOT(int);
	void sliderMovedSLOT(int);
	void seekStartSLOT();
	void seekEndSLOT();

	
};

#endif // VIDEOPLAYCONTROLSWIDGET_H
