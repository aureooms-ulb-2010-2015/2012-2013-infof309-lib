#include "InputChoiceWidget.h"
#include <QFileDialog>
#include "VideoCaptureDeviceDetectorForLinux.h"
#include <QListView>
#include <QStyledItemDelegate>

// add per url http://192.168.2.29/video.cgi?resolution=640x480&req_fps=10&.mjpg

InputChoiceWidget::InputChoiceWidget():_lastVisitedFolder(QDir::homePath()){
    this->addItem("Choisir la source");
    this->addItem("Ouvrir un fichier");
    this->addItem("Se connecter à une caméra réseau");

	QFile file2(":/qss/QComboBox.qss");
	file2.open(QFile::ReadOnly);
	QString styleSheet2 = QLatin1String(file2.readAll());
	this->setStyleSheet(styleSheet2);
	file2.close();


	QObject::connect(this,SIGNAL(currentIndexChanged(int)),this,SLOT(currentIndexChangedSLOT(int)));
}

QString InputChoiceWidget::getVideoFilePath(){
	QString filePath = QFileDialog::getOpenFileName(this, tr("Open Video File"),this->_lastVisitedFolder,tr("Video (*.avi *.mpeg *.mov *.mp4 *.wmv *.mkv);;Others (*.*)"));
	for(int i = filePath.length(); i > 0; --i){
		if(filePath.at(i-1) == QChar('/')){
			this->_lastVisitedFolder = filePath.left(i);
			break;
		}
	}
	return filePath;
}

void InputChoiceWidget::showPopup(){
	this->refreshDevices();
	QComboBox::showPopup();
}

void InputChoiceWidget::refreshDevices(){
    while(this->count() > VIDEO_STREAM_BEGIN){
        this->removeItem(VIDEO_STREAM_BEGIN);
	}
	VideoCaptureDeviceDetectorForLinux dtc = VideoCaptureDeviceDetectorForLinux();
	QList<VideoCaptureDeviceInfo> infoList = dtc.getCaptureDevicesInfo();
	for(int i = 0; i < infoList.size(); ++i){
		this->addItem(infoList.at(i).name);
    }
}

void InputChoiceWidget::chooseNetworkStream(){
	if (dialog->exec() == QDialog::Accepted) {
		QString url = dialog->getResult();
		this->setItemText(0,url);
		emit currentSourceChanged(url);
	}
    this->setCurrentIndex(0);
}

void InputChoiceWidget::currentIndexChangedSLOT( int index ){
	if(index == 1){
		QString fileName = this->getVideoFilePath();
		if(!fileName.isEmpty()){
			this->setItemText(0,fileName);
			emit currentSourceChanged(fileName);
		}
		this->setCurrentIndex(0);
	}
    if(index == 2){
        this->chooseNetworkStream();
    }
    else if(index >= VIDEO_STREAM_BEGIN){
		this->setItemText(0,this->currentText());
        emit currentSourceChanged(index-VIDEO_STREAM_BEGIN);
		this->setCurrentIndex(0);
	}
}
