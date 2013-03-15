#ifndef ALGORITHM_CUSTOMCONDENSATIONV1_H
#define ALGORITHM_CUSTOMCONDENSATIONV1_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Algorithm_CustomCondensation.h"

#include "Matcher_TargetMatcherV2_BinaryMask.h"
#include "Target_CondensationTargetV1.h"

#include <chrono>
#include <vector>
#include <random>
#include "Algorithm_Sub_Tagging.h"

class CustomCondensationV1: public CustomCondensation<TargetMatcherV2_BinaryMask>{

public:

	typedef std::chrono::high_resolution_clock Clock;
	typedef cv::Point Point;
	typedef std::vector<Point> Points;
	typedef cv::Rect Rect;
	typedef std::vector<Rect> Rects;
	typedef TargetMatcherV2_BinaryMask::Target Target;
	typedef Target::Density Density;
	typedef Target::Densities Densities;
	typedef std::vector<Target> Targets;
	typedef std::discrete_distribution<int> DiscreteDistribution;
	typedef struct{
		DiscreteDistribution x;
		DiscreteDistribution y;
		DiscreteDistribution w;
		DiscreteDistribution h;
	} DiscreteDistributions;
	typedef std::uniform_int_distribution<int> UniformDistribution;
	typedef struct{
		UniformDistribution w;
		UniformDistribution h;
	} UniformDistributions;

	typedef std::default_random_engine Generator;
	typedef int Delta;
	typedef struct{
		Delta x, y, w, h;
	} Deltas;

private:

	Generator generator = Generator(Clock::now().time_since_epoch().count());
	Target::Data model = cv::imread("data/bigfish.png");//("simo.png");
	Targets currentlyTracked;
	size_t pollingRange = 10;
	size_t generatingRange = 1;
	int spreadRange = 20;
	int minScore = 50;
	Densities density;
	Sub_Tagging tagging;

	virtual Points generatePoints(const cv::Mat& in);
	virtual Rects generateRects(const cv::Mat& in);
	virtual Rects generateRects(const cv::Mat& in, DiscreteDistributions &distributions);

	void initDensity(const cv::Mat &in);

	// compute and update target properties
	void ConDensAte(const cv::Mat &in, cv::Mat &out, Target& target);

	void pollNewTargets(const cv::Mat& in, cv::Mat &out);

	//void computeDensity(const cv::Mat& in, Rects rects);
	//void computeDensity(const cv::Mat& in, const Target &target);

	Deltas shift(const Densities&);
	void spread(Deltas deltas);

	static Delta shift(Density&, const Density&);
	static void spread(Density&, Delta delta, int spreadRange);

public:


	CustomCondensationV1();
	void process(const cv::Mat &in, cv::Mat &out);


};

#endif // ALGORITHM_CUSTOMCONDENSATIONV1_H
