#include "ProcessingChoiceWidget.h"

#include "InputChoiceWidget.h"
#include <QFileDialog>
#include <QListView>
#include <QStyledItemDelegate>
#include <QApplication>

ProcessingChoiceWidget::ProcessingChoiceWidget(){
	QFile file(":/qss/QComboBox.qss");
	file.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(file.readAll());
	this->setStyleSheet(styleSheet);
	file.close();
}


void ProcessingChoiceWidget::keyPressEvent(QKeyEvent *event){
	if(QApplication::keyboardModifiers() & Qt::ControlModifier){
		return event->ignore();
	}
	return QComboBox::keyPressEvent(event);
}
