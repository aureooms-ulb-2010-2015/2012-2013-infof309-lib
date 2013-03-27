#ifndef TARGET_CondensationTargetV3_H
#define TARGET_CondensationTargetV3_H

#include <array>
#include <opencv2/core/core.hpp>

class CondensationTargetV3{
public:

	typedef struct{
		float mean;
		float deviation;
	} Density;
	typedef struct{
		Density x;
		Density y;
	} Densities;
	typedef struct{
		Densities density;
		cv::Point point;
		cv::Point initial;
		int trust;
		cv::Point previous;
	} Feature;
	typedef std::vector<Feature> Features;

public:
	Features features;

	CondensationTargetV3(){}

};


#endif // TARGET_CondensationTargetV3_H
