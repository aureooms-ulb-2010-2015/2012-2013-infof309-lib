#ifndef TAGGING_H
#define TAGGING_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
#include "FrameProcessor.h"

#include "Algorithm_Sub_Tagging.h"


class Tagging : public FrameProcessor {
private:
	typedef Sub_Tagging::Rects Rects;

	Sub_Tagging tagger;

public:

	Tagging(){}
	Tagging(int threshold, double learningRate, cv::Size blurRange):
		tagger(threshold,learningRate, blurRange){}

    // processing method
	void process(const cv::Mat &in, cv::Mat &out) {
		out = in.clone();

		Rects rects = tagger.process(in);
		for(const cv::Rect& rect : rects){
			cv::rectangle(out, rect, cv::Scalar(255,0,0), 1,8,0 );
		}
    }
};

#endif // TAGGING_H
