#ifndef ALGORITHM_TAGANTRACK_H
#define ALGORITHM_TAGANTRACK_H

#include "FrameProcessor.h"
#include "Algorithm_Tagging.h"
#include "Algorithm_FeaturesTracker.h"
#include "Algorithm_Sub_BinaryMask.h"

class TagNTrack : public FrameProcessor {

    Tagging *_tag;
    FeatureTracker *_track;
    Sub_BinaryMask *_mask;
    /*
    Tagging *_tag = new Tagging();
    FeatureTracker *_track = new FeatureTracker();
    Sub_BinaryMask *_mask = new Sub_BinaryMask();
    */

    cv::Mat _in;
    cv::Mat _out;
    cv::Mat _temp;
public:
    TagNTrack(){
        _tag=new Tagging();
        _track=new FeatureTracker();
        _mask=new Sub_BinaryMask();
    }

    void process(const cv::Mat &input, cv::Mat &output){
        _mask->process(input,output);
        _tag->process(output,_out);
        _track->process(_out,output);
    }
};


#endif // ALGORITHM_TAGANTRACK_H
