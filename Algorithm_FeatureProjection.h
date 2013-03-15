#ifndef ALGORITHM_FEATUREPROJECTION_H
#define ALGORITHM_FEATUREPROJECTION_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "FrameProcessor.h"
#include "Target_FeatureProjectionTarget.h"
#include "Algorithm_Sub_Tagging.h"


class FeatureProjection : public FrameProcessor {
public:
	static const size_t D = 5;
	typedef FeatureProjectionTarget<D> Target;
	typedef std::vector<Target> Targets;
	typedef std::pair<size_t,size_t> Match;
	typedef std::vector<Match> Matches;
	typedef int Score;
	typedef std::pair<Score, Match> MatchingScore;
	typedef std::multimap<Score, Match> MatchingScores;
	typedef Target::Feature Feature;

private:
	uint32_t NEXT_ID = ~0;

	Targets _previousTargets;
	Sub_Tagging tagging;

	const int AGE_TRUST = 64;
	const int AGE_MID = 24;
	const int AGE_DIE = 12;
	const int AGE_PENALTY = 1;
	const int AGE_BONUS = 2;
	const int MAXSIZE = 40;
	const int MINSIZE = 5;

	const int MAXDIST = 10;
	const int MAXTRANSFORM = 10;


public:

	FeatureProjection();
	virtual void process(const cv::Mat &in, cv::Mat &out);

	virtual MatchingScores generateMatchingScores(const Targets&) const;
	virtual Matches generateMatches(const MatchingScores&, size_t) const;
	virtual void writeFishCount(cv::Mat&, int);

	static void drawTarget(cv::Mat& out, const Target& target, cv::Scalar color);
};

#endif // ALGORITHM_FEATUREPROJECTION_H
