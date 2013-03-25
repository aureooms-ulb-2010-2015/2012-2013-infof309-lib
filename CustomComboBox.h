#ifndef CUSTOMCOMBOBOX_H
#define CUSTOMCOMBOBOX_H

#include <QComboBox>
#include <QKeyEvent>

class CustomComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit CustomComboBox(QWidget *parent = 0);

	void keyPressEvent(QKeyEvent *event);
signals:
    
public slots:
    
};

#endif // CUSTOMCOMBOBOX_H
