#ifndef CONDENSATIONPARAMETERS_H
#define CONDENSATIONPARAMETERS_H

#include <cstring>
#include <memory>
#include "Matcher_DistanceMatcher.h"
#include "Matcher_GreyLevelDistanceMatcher.h"
#include "Matcher_GradientDistanceMatcher.h"

class CondensationParameters{
public:

	size_t pollingRange = 50;
	size_t generatingRange = 1;
	int spreadRange = 5;
	int TARGET_MIN_WIDTH = 50;
	int TARGET_MIN_HEIGHT = 50;
	int TARGET_MAX_WIDTH = 300;
	int TARGET_MAX_HEIGHT = 300;
	size_t MAX_DIST = 200;
	size_t maxCorners = 30;
	double qualityLevel = 0.01;
	double minDistance = 10;
	int blockSize = 3;
	bool useHarrisDetector = false;
	double k = 0.04;
	size_t MIN_ACCUMULATOR_ITERATIONS = 45;
	unsigned int R = 6;
	int matcherIndex = 0;
	std::shared_ptr<DistanceMatcher> matcher = std::shared_ptr<DistanceMatcher>(new GreyLevelDistanceMatcher(R));

	int TRUST_START = 100;
	int TRUST_DIE = 0;
	int TRUST_BONUS_FACTOR = 10;
	int TRUST_MALUS = 1;
	size_t MIN_FEATURES = 5;

	CondensationParameters();
};

#endif // CONDENSATIONPARAMETERS_H
