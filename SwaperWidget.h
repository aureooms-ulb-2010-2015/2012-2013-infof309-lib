#ifndef WIDGETSWAPER_H
#define WIDGETSWAPER_H

#include <QWidget>
#include <QLabel>

class WidgetSwaper : public QWidget
{
    Q_OBJECT

private:
    QWidget* _widget1;
    QWidget* _widget2;
    int _state;

public:
    enum State { first, second };
    explicit WidgetSwaper(QWidget *parent, QWidget*, QWidget*);
	void swap();
	void setFirstState();
	void setSecondState();
	bool isFirstState();
	bool isSecondState();

protected:
    void mousePressEvent(QMouseEvent* event);

signals:
    void clickedStateFirst();
    void clickedStateSecond();
};

#endif // WIDGETSWAPER_H
