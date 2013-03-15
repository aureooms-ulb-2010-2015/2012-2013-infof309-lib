#ifndef ALGORITHM_BINARYMASKWITHORIGINALFRAME_H
#define ALGORITHM_BINARYMASKWITHORIGINALFRAME_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "FrameProcessor.h"
#include "Algorithm_Sub_BinaryMask.h"

class BinaryMaskWithOriginalFrame : public FrameProcessor {
private:
	Sub_BinaryMask _mask;

public:

	BinaryMaskWithOriginalFrame();
	void process(const cv::Mat &in, cv::Mat &out);
};

#endif // ALGORITHM_BINARYMASKWITHORIGINALFRAME_H
