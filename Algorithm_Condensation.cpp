#include "Algorithm_Condensation.h"

#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

#define drawCross(img, center, color, d )                  \
  line( img, cv::Point( center.x - d, center.y - d ),           \
	cv::Point( center.x + d, center.y + d ), color, 2, CV_AA, 0);   \
  line( img, cv::Point( center.x + d, center.y - d ),           \
	cv::Point( center.x - d, center.y + d ), color, 2, CV_AA, 0 )

Condensation::Condensation():measurement(2,1){
	measurement.setTo(cv::Scalar(0));

	int dim = 2;
	int nParticles = 25;
	xRange = 650.0;
	yRange = 650.0;

	float minRange[] = { 0, 0 };
	float maxRange[] = { xRange, yRange };
	cvInitMatHeader(&LB, 2, 1, CV_32FC1, minRange);
	cvInitMatHeader(&UB, 2, 1, CV_32FC1, maxRange);

	condens = cvCreateConDensation(dim, dim, nParticles);
	cvConDensInitSampleSet(condens, &LB, &UB);

	// The OpenCV documentation doesn't tell you to initialize this
	// transition matrix, but you have to do it.  For this 2D example,
	// we're just using a 2x2 identity matrix.  I'm sure there's a slicker
	// way to do this, left as an exercise for the reader.
	condens->DynamMatr[0] = 1.0;
	condens->DynamMatr[1] = 0.0;
	condens->DynamMatr[2] = 0.0;
	condens->DynamMatr[3] = 1.0;
}


void Condensation::process(const cv::Mat &in, cv::Mat &out) {
	out = in.clone();

	mouseV.clear();
	particleV.clear();


	measurement(0) = 0;//WTF?
	measurement(1) = 0;

	cv::Point measPt(measurement(0),measurement(1));
	mouseV.push_back(measPt);

	for (int i = 0; i < condens->SamplesNum; i++) {

		float diffX = (measurement(0) - condens->flSamples[i][0])/xRange;
		float diffY = (measurement(1) - condens->flSamples[i][1])/yRange;

		condens->flConfidence[i] = 1.0 / (sqrt(diffX * diffX + diffY * diffY));

		cv::Point partPt(condens->flSamples[i][0], condens->flSamples[i][1]);
		drawCross(out, partPt , cv::Scalar(255,0,255), 2);

	}

	cvConDensUpdateByTime(condens);

	cv::Point statePt(condens->State[0], condens->State[1]);
	particleV.push_back(statePt);

	// plot points
	drawCross(out, statePt, cv::Scalar(255,255,255), 5 );
	drawCross(out, measPt, cv::Scalar(0,0,255), 5 );

	for (size_t i = 0; i < mouseV.size() - 1; i++) {
		cv::line(out, mouseV[i], mouseV[i+1], cv::Scalar(255,255,0), 1);
	}

	for (size_t i = 0; i < particleV.size() - 1; i++) {
		cv::line(out, particleV[i], particleV[i+1], cv::Scalar(0,255,0), 1);
	}

	return;
}
