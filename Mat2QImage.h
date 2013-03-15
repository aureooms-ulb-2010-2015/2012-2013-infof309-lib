#ifndef MAT2QIMAGE_H
#define MAT2QIMAGE_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QImage>
class Mat2QImage
{
public:
	Mat2QImage();
	static QImage exec(const cv::Mat &src);
	static QImage exec(const cv::Mat3b &src);
	static QImage exec(const cv::Mat_<double> &src);
};

#endif // MAT2QIMAGE_H
