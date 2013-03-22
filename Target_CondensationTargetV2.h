#ifndef TARGET_CondensationTargetV2_H
#define TARGET_CondensationTargetV2_H

#include <array>
#include <opencv2/core/core.hpp>

class CondensationTargetV2{
public:
	typedef size_t Score;
	typedef std::vector<Score> Density;
	typedef struct{
		Density x;
		Density y;
	} Densities;
	typedef struct{
		Densities density;
		cv::Point point;
		//cv::Point initial;
	} Feature;
	typedef std::vector<Feature> Features;

public:
	Features features;

	CondensationTargetV2(){}

	CondensationTargetV2(const Features& features) : features(features){}

};


#endif // TARGET_CondensationTargetV2_H
