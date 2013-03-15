#include "CustomComboBox.h"
#include <QFile>


CustomComboBox::CustomComboBox(QWidget *parent) :
    QComboBox(parent){
    QFile temp(":/qss/QComboBox.qss");
    temp.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(temp.readAll());
    this->setStyleSheet(styleSheet);
    temp.close();
}
