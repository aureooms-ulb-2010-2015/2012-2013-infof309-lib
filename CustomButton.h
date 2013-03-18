#ifndef CUSTOMBUTTON_H
#define CUSTOMBUTTON_H

#include <QPushButton>

class CustomButton : public QPushButton{
	Q_OBJECT
public:
	explicit CustomButton(QWidget *parent = 0);
	explicit CustomButton(const QString &text, QWidget *parent = 0);
	
signals:
public slots:
	
};

#endif // CUSTOMBUTTON_H
