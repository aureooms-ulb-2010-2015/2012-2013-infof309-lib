#ifndef INPUTCHOICEWIDGET_H
#define INPUTCHOICEWIDGET_H

#include <QComboBox>
#include "NetworkStreamInputDialog.h"

class InputChoiceWidget : public QComboBox{
	Q_OBJECT
private:
	const int VIDEO_STREAM_BEGIN = 3;
	QString _lastVisitedFolder;
	NetworkStreamInputDialog* dialog = new NetworkStreamInputDialog(this);
    virtual void chooseNetworkStream();

public:
	InputChoiceWidget();
	QString getVideoFilePath();
	virtual void showPopup();
    virtual void refreshDevices();

public slots:
	void currentIndexChangedSLOT(int);

signals:
	void currentSourceChanged(int);
	void currentSourceChanged(QString);

};

#endif // INPUTCHOICEWIDGET_H
