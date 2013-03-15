#ifndef TIMEDISPLAYERWIDGET_H
#define TIMEDISPLAYERWIDGET_H
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>

class TimeDisplayerWidget : public QWidget
{
    Q_OBJECT

private:
	QLabel* _elapsed, *_total, *_separator;
    QHBoxLayout* _layout;

public:
	explicit TimeDisplayerWidget(QWidget *parent = 0);
    void setElapsedTime(int);
    void setTotalTime(int);
signals:
    
public slots:
    
};

#endif // TIMEDISPLAYERWIDGET_H
