#include "CustomButton.h"

CustomButton::CustomButton(QWidget *parent) :
	QPushButton(parent)
{
}

CustomButton::CustomButton(const QString &text, QWidget *parent) : QPushButton(text, parent)
{
}
