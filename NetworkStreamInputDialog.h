#ifndef NETWORKSTREAMINPUTDIALOG_H
#define NETWORKSTREAMINPUTDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include "CustomButton.h"
#include "CustomTextInput.h"
#include "CustomComboBox.h"


// add per url http://192.168.2.29/video.cgi?resolution=640x480&req_fps=10&.mjpg

class NetworkStreamInputDialog : public QDialog{
    Q_OBJECT
private:
	QGridLayout* _layout  = new QGridLayout(this);
	QWidget* _firstLine = new QWidget();
	QHBoxLayout* _firstLineLayout = new QHBoxLayout(this->_firstLine);
	CustomComboBox* _protocol = new CustomComboBox();
	QLabel* _sep0 = new QLabel("://");
	CustomTextInput* _hostname = new CustomTextInput();
	QLabel* _sep1 = new QLabel(":");
	CustomTextInput* _port = new CustomTextInput();
	QLabel* _sep2 = new QLabel("/");
	CustomTextInput* _ressource = new CustomTextInput();

	QLabel* _resolutionLabel = new QLabel("Résolution");
	CustomComboBox* _resolution = new CustomComboBox();
	QLabel* _fpsLabel = new QLabel("Fps");
	CustomComboBox* _fps = new CustomComboBox();
	QLabel* _formatLabel = new QLabel("Format");
	CustomComboBox* _format = new CustomComboBox();

	CustomButton* _confirmButton = new CustomButton("Se connecter");
	CustomButton* _cancelButton = new CustomButton("Annuler");

	QString _result;
	virtual QString generateUrl();


public:
	explicit NetworkStreamInputDialog(QWidget *parent = 0);

	virtual QString getResult();
	virtual void accept();
    
signals:
    
public slots:
    
};

#endif // NETWORKSTREAMINPUTDIALOG_H
