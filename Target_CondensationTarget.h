#ifndef TARGET_CONDENSATIONTARGET_H
#define TARGET_CONDENSATIONTARGET_H

#include <array>
#include <opencv2/core/core.hpp>

class CondensationTarget{
public:
	typedef std::vector<int> Density;
	typedef struct{
		Density x;
		Density y;
		Density w;
		Density h;
	} Densities;

protected:
	cv::Size size;


public:
	cv::Rect rect;
	Densities density;

	CondensationTarget(cv::Size size, cv::Rect r) : size(size), rect(r){
		density.x.reserve(size.width);
		density.y.reserve(size.height);
		density.w.reserve(size.width);
		density.h.reserve(size.height);
	}

	CondensationTarget(cv::Size size, cv::Rect r, const Densities& s) :
		size(size), rect(r), density(s){}

};

#endif // TARGET_CONDENSATIONTARGET_H
