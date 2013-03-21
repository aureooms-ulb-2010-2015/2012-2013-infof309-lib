#ifndef BASICWINDOW_H
#define BASICWINDOW_H

#include <QMainWindow>
#include "VideoStreamReader.h"
#include "VideoPlayControlsWidget.h"
#include "StreamDisplayer.h"
#include "InputChoiceWidget.h"
#include "ProcessingChoiceWidget.h"
#include "ImageDisplayerWidget.h"
#include "FrameProcessor.h"
#include "VideoStreamProcessor.h"
#include <QKeyEvent>
#include "ImageDisplayerWidgetImpl.h"

class BasicWindow : public QWidget{
	Q_OBJECT

protected:

	VideoStreamInfo _streamInfo;

	QWidget* _wraper = new QWidget(this);
	QGridLayout* _layout = new QGridLayout(_wraper);

	VideoStreamReader* _streamReader = new VideoStreamReader();
	FrameProcessor* _frameProcessor = NULL;
	VideoStreamProcessor* _streamProcessor = new VideoStreamProcessor(_streamReader,_frameProcessor);
	StreamDisplayer* _displayer = new StreamDisplayer(_streamProcessor);

	VideoPlayControlsWidget* _videoControls = new VideoPlayControlsWidget();
	ImageDisplayerWidget* _videoFrame = new ImageDisplayerWidgetImpl();
	QWidget* _upperBar = new QWidget();
	InputChoiceWidget* _inputChoice = new InputChoiceWidget();

	void sourceReady();
	void centerWindow();
	void reloadStream();
	void playPauseToggle();

	int lastDevice;
	QString lastPath;
	int whichLast = -1;

	virtual FrameProcessor* generateProcessor() = 0;

public:
	explicit BasicWindow(QWidget *parent = 0);
	virtual ~BasicWindow();
	virtual void keyPressEvent(QKeyEvent *);

public slots:
	void requestChangeSourceSLOT(int);
	void requestChangeSourceSLOT(QString);
	void posChangedSLOT(int);
	void timeChangedSLOT(int);
	void requestEnterFullScreenSLOT();
	void requestExitFullScreenSLOT();
};


#endif // BASICWINDOW_H
