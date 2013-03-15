#ifndef TARGET_FEATUREPROJECTIONTARGET_H
#define TARGET_FEATUREPROJECTIONTARGET_H

#include <string>
#include <opencv2/core/core.hpp>

template<size_t D> class FeatureProjectionTarget{

public:
	typedef double Feature;

	std::string id;
	Feature x[D];
	Feature y[D];
	cv::Rect rect;
	int age;

	FeatureProjectionTarget(cv::Rect rect):rect(rect), age(0){
		x[0] = rect.x + rect.width/2;
		y[0] = rect.y + rect.height/2;
		for(size_t i = 1; i < D; ++i){
			x[i] = y[i] = 0;
		}
	}
};

#endif // TARGET_FEATUREPROJECTIONTARGET_H
