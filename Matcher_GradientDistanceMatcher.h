#ifndef MATCHER_GRADIENTDISTANCEMATCHER_H
#define MATCHER_GRADIENTDISTANCEMATCHER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>
#include "Matcher_DistanceMatcher.h"

class GradientDistanceMatcher : public DistanceMatcher{


public:
	typedef DistanceMatcher::Distance Distance;
	GradientDistanceMatcher(int radius):DistanceMatcher(radius){}

	virtual ~GradientDistanceMatcher() throw(){}
	virtual Distance computeDistance(const cv::Mat& prev_img, const cv::Mat& img,const cv::Point& a, const cv::Point& b){
		if(a.x - R < 0 || a.y - R < 0 || b.x - R < 0 || b.y - R < 0 ||
				a.x + R + 1 > img.cols || a.y + R + 1> img.rows ||
				b.x + R + 1 > img.cols || b.y + R + 1> img.rows) return ~0;
		cv::Rect roi[2];
//		roi[0].x = std::max(a.x - R, 0);
//		roi[0].y = std::max(a.y - R, 0);
//		roi[0].width  = (a.x + R + 1 > img.cols)? R + img.cols - a.x : R*2+1;
//		roi[0].height = (a.y + R + 1> img.rows)? R + img.rows - a.y : R*2+1;
//		roi[1].x = std::max(b.x - R, 0);
//		roi[1].y = std::max(b.y - R, 0);
//		roi[1].width  = (b.x + R + 1> img.cols)? R + img.cols - b.x : R*2+1;
//		roi[1].height = (b.y + R + 1> img.rows)? R + img.rows - b.y : R*2+1;

//		roi[0].width  =  roi[1].width = std::min( roi[0].width,  roi[1].width);
//		roi[0].height = roi[1].height = std::min(roi[0].height, roi[1].height);

		roi[0].x = a.x - R;
		roi[0].y = a.y - R;
		roi[1].x = b.x - R;
		roi[1].y = b.y - R;
		roi[0].width = roi[1].width = R*2+1;
		roi[0].height = roi[1].height = R*2+1;

		cv::Mat grey[2];
		//std::cout << "x : " << roi[0].x  <<", y : " << roi[0].y  <<", width : " << roi[0].width  <<", height : " << roi[0].height  << std::endl;
		//std::cout << "x : " << roi[1].x  <<", y : " << roi[1].y  <<", width : " << roi[1].width  <<", height : " << roi[1].height  << std::endl;
		grey[0] = prev_img(roi[0]);
		grey[1] = img(roi[1]);

		int gradient[2][2] = {{0,0},{0,0}};

		const uchar* ptr[2];
		ptr[0] = grey[0].ptr();
		ptr[1] = grey[1].ptr();
		for(int i = 0; i < (R*2+1); ++i){
			for(int j = 0; j < R; ++j){
				gradient[0][0] += ptr[0][i*(R*2+1)+j] - ptr[0][(i+1)*(R*2+1)-j-1];
				gradient[0][1] += ptr[0][i+(R*2+1)*j] - ptr[0][i+(2*R-j)*(2*R+1)];
				gradient[1][0] += ptr[1][i*(R*2+1)+j] - ptr[1][(i+1)*(R*2+1)-j-1];
				gradient[1][1] += ptr[1][i+(R*2+1)*j] - ptr[1][i+(2*R-j)*(2*R+1)];
			}
		}

		int delta[2];
		delta[0] = gradient[0][0] - gradient[1][0];
		delta[1] = gradient[0][1] - gradient[1][1];

		Distance distance = delta[0]*delta[0] + delta[1]*delta[1];
		return sqrt(distance);
	}
};

#endif // MATCHER_GRADIENTDISTANCEMATCHER_H
