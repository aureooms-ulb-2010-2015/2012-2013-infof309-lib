#ifndef VIDEOCAPTUREDEVICEDETECTORFORLINUX_H
#define VIDEOCAPTUREDEVICEDETECTORFORLINUX_H

#include "VideoCaptureDeviceDetector.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <libv4l2.h>
#include <linux/videodev2.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <ctype.h>

#include <sys/time.h>
#include <sys/ioctl.h>

class VideoCaptureDeviceDetectorForLinux : public VideoCaptureDeviceDetector{

public:

	QList<VideoCaptureDeviceInfo> getCaptureDevicesInfo(){
		QList<VideoCaptureDeviceInfo> result;
		int fd;
		int n = 0;
		do{
			VideoCaptureDeviceInfo info;
			QString device = "/dev/video";
			device.append(QString::number(n));
			fd = open(device.toStdString().c_str(),O_RDONLY);
			if (-1 == fd) {
				 break;
			};

			struct v4l2_capability  capability;
			memset(&capability,0,sizeof(capability));
			if (-1 == ioctl(fd,VIDIOC_QUERYCAP,&capability)){
				continue;
			}
			info.name = (const char*)capability.card;
			info.index = n;
			result.append(info);
			++n;
		}while(n>0);
		return result;
	}

};
#endif // VIDEOCAPTUREDEVICEDETECTORFORLINUX_H
