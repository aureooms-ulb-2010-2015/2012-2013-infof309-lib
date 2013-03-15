#include "Matcher_TargetMatcherV1.h"

TargetMatcherV1::TargetMatcherV1(){
	matcher.setConfidenceLevel(0.98);
	matcher.setMinDistanceToEpipolar(1.0);
	matcher.setRatio(0.65f);
	cv::Ptr<cv::FeatureDetector> featureDetector = new cv::SurfFeatureDetector(10);
	matcher.setFeatureDetector(featureDetector);
}

TargetMatcherV1::~TargetMatcherV1() throw(){}

int TargetMatcherV1::computeScore(const Target::Data &a, const Target::Data &b){
	std::vector<cv::DMatch> matches;
	std::vector<cv::KeyPoint> keypoints1, keypoints2;
	cv::Mat img_a = a;
	cv::Mat img_b = b;
	matcher.match(img_a, img_b, matches, keypoints1, keypoints2);
	return matches.size();
}
