#ifndef CONDENSATIONPARAMETERS_H
#define CONDENSATIONPARAMETERS_H

#include <cstring>

class CondensationParameters{
public:

	size_t pollingRange = 50;
	size_t generatingRange = 1;
	int spreadRange = 20;
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

	CondensationParameters();
};

#endif // CONDENSATIONPARAMETERS_H
