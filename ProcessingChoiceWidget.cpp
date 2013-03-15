#include "ProcessingChoiceWidget.h"

#include "InputChoiceWidget.h"
#include <QFileDialog>
#include <QListView>
#include <QStyledItemDelegate>

ProcessingChoiceWidget::ProcessingChoiceWidget(){
	QFile file(":/qss/QComboBox.qss");
	file.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(file.readAll());
	this->setStyleSheet(styleSheet);
	file.close();
}
