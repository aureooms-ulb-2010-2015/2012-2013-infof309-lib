#include "Mat2QImage.h"

Mat2QImage::Mat2QImage(){}

QImage Mat2QImage::exec(const cv::Mat &src){
	QImage qtFrame(src.data, src.size().width, src.size().height, src.step, QImage::Format_RGB888);
	return qtFrame.rgbSwapped();
}


QImage Mat2QImage::exec(const cv::Mat3b &src) {
	QImage dest(src.cols, src.rows, QImage::Format_ARGB32);
	for (int y = 0; y < src.rows; ++y) {
		const cv::Vec3b *srcrow = src[y];
		QRgb *destrow = (QRgb*)dest.scanLine(y);
		for (int x = 0; x < src.cols; ++x) {
				destrow[x] = qRgba(srcrow[x][2], srcrow[x][1], srcrow[x][0], 255);
		}
	}
	return dest;
}

QImage Mat2QImage::exec(const cv::Mat_<double> &src){
	double scale = 255.0;
	QImage dest(src.cols, src.rows, QImage::Format_ARGB32);
	for (int y = 0; y < src.rows; ++y) {
		const double *srcrow = src[y];
		QRgb *destrow = (QRgb*)dest.scanLine(y);
		for (int x = 0; x < src.cols; ++x) {
				unsigned int color = srcrow[x] * scale;
				destrow[x] = qRgba(color, color, color, 255);
		}
	}
	return dest;
}


