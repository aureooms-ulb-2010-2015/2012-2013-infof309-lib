#ifndef PROCESSINGCHOICEWIDGET_H
#define PROCESSINGCHOICEWIDGET_H

#include <QComboBox>
#include <QKeyEvent>

class ProcessingChoiceWidget : public QComboBox{
public:
	ProcessingChoiceWidget();

	void keyPressEvent(QKeyEvent *event);
};
#endif // PROCESSINGCHOICEWIDGET_H
