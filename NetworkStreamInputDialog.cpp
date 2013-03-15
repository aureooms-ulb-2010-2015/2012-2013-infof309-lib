#include "NetworkStreamInputDialog.h"
#include <QObject>

NetworkStreamInputDialog::NetworkStreamInputDialog(QWidget *parent) :
	QDialog(parent){
	this->_layout->addWidget(this->_firstLine,0,0,1,4);
	this->_layout->addWidget(this->_resolutionLabel,2,1,1,1);
	this->_layout->addWidget(this->_resolution,2,2,1,1);
	this->_layout->addWidget(this->_fpsLabel,3,1,1,1);
	this->_layout->addWidget(this->_fps,3,2,1,1);
	this->_layout->addWidget(this->_formatLabel,4,1,1,1);
	this->_layout->addWidget(this->_format,4,2,1,1);
	this->_layout->addWidget(this->_confirmButton,6,1,1,1);
	this->_layout->addWidget(this->_cancelButton,6,2,1,1);

	this->_resolution->addItem("320x240");
	this->_resolution->addItem("640x480");
	this->_resolution->setCurrentIndex(1);
	this->_fps->addItem("10");
	this->_fps->addItem("25");
	this->_fps->addItem("30");
	this->_fps->setCurrentIndex(2);
	this->_format->addItem(".mjpg");

	this->_firstLineLayout->addWidget(this->_protocol);
	this->_firstLineLayout->addWidget(this->_sep0);
	this->_firstLineLayout->addWidget(this->_hostname);
	this->_firstLineLayout->addWidget(this->_sep1);
	this->_firstLineLayout->addWidget(this->_port);
	this->_firstLineLayout->addWidget(this->_sep2);
	this->_firstLineLayout->addWidget(this->_ressource);

	this->_protocol->addItem("http");
	this->_protocol->addItem("rtsp");

	this->_hostname->setPlaceholderText("Adresse IP");
	this->_port->setPlaceholderText("Port (par défaut)");
	this->_ressource->setPlaceholderText("Ressource (par défaut)");


	QObject::connect(this->_confirmButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

QString NetworkStreamInputDialog::getResult(){
	return this->_result;
}

QString NetworkStreamInputDialog::generateUrl(){
	QString protocol = this->_protocol->currentText();
	QString hostname = this->_hostname->text();
	QString port = this->_port->text();
	QString ressource = this->_ressource->text();
	QString resolution = this->_resolution->currentText();
	QString fps = this->_fps->currentText();
	QString format = this->_format->currentText();

	QString url = "";

	url += protocol + "://";
	url += hostname;
	if(port != ""){
		url += ":" + port;
	}
	if(ressource != ""){
		url += "/" + ressource;
	}
	url += "?";
	url += "resolution=" + resolution;
	url += "&";
	url += "req_fps=" + fps;
	url += "&";
	url += format;

	return url;
}

void NetworkStreamInputDialog::accept(){
	this->_result = this->generateUrl();
	QDialog::accept();
}
