#ifndef ALGORITHM_OOMSCHALLENGE_H
#define ALGORITHM_OOMSCHALLENGE_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "FrameProcessor.h"
#include "Target.h"


class OomsChallenge : public FrameProcessor {
private:
	std::vector<Target> _previousTargets;

	cv::Mat _gray;			// current gray-level image
	cv::Mat _background;	// accumulated background
	cv::Mat _backImage;		// background image
	cv::Mat _foreground;		// foreground image


	std::vector<cv::Vec4i> _hierarchy;

	cv::RNG _rng;

	int _threshold;			// threshold for foreground extraction
	double _learningRate;    // learning rate in background accumulation


public:

	OomsChallenge();
	virtual void process(const cv::Mat &in, cv::Mat &out);

	virtual std::multimap<int, std::pair<size_t,size_t> > generateMatchingScores(const std::vector<Target>&) const;
	virtual std::vector<std::pair<size_t,size_t> > generateMatches(const std::multimap<int, std::pair<size_t,size_t> >&, size_t) const;
	virtual void writeFishCount(cv::Mat&, int);
};
#endif // ALGORITHM_OOMSCHALLENGE_H
