#ifndef PLAYPAUSEBUTTONWIDGET_H
#define PLAYPAUSEBUTTONWIDGET_H

#include <QWidget>
#include "SwaperWidget.h"

class PlayPauseButtonWidget : public QWidget{
	Q_OBJECT

private:
    WidgetSwaper* _button;
public:
	explicit PlayPauseButtonWidget(QWidget *parent = 0);
	void setPause();
	void setPlay();
	bool isPause();
	bool isPlay();

signals:

    void pause();
    void play();
};

#endif // PLAYPAUSEBUTTONWIDGET_H
