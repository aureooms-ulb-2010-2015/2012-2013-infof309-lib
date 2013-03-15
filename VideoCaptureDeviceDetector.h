#ifndef VIDEOCAPTUREDEVICEDETECTOR_H
#define VIDEOCAPTUREDEVICEDETECTOR_H

#include <QString>
#include <QList>

class VideoCaptureDeviceInfo{
public:
	int index;
	QString name;
	VideoCaptureDeviceInfo():index(-1), name("error"){}

};

class VideoCaptureDeviceDetector{

public:
	virtual QList<VideoCaptureDeviceInfo> getCaptureDevicesInfo() = 0;
};

#endif // VIDEOCAPTUREDEVICEDETECTOR_H
