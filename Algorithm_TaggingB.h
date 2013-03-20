#ifndef ALGORITHM_TAGGINGB_H
#define ALGORITHM_TAGGINGB_H


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
#include "FrameProcessor.h"
#include "Algorithm_Sub_TaggingB.h"

class TaggingB : public FrameProcessor {

private:

    typedef cv::Rect Rect;
    typedef std::vector<Rect> Rects;
    Sub_TaggingB tagging;
    cv::Scalar color = cv::Scalar(0,0x80,0);

public:

    TaggingB(){
        std::cout<<"tagging b"<<std::endl;
    }

    // processing method
    void process(const cv::Mat &in, cv::Mat &out) {
        std::cout<<"tagging b process"<<std::endl;
        out = in.clone();
        Rects rects = tagging.process(in);
        //cv::putText(out, s.str(), cv::Point(100,100), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.8, cv::Scalar(255,200,200), 1, CV_AA);
        int i = 0;
        for(const Rect& rect : rects){
            i++;
            cv::rectangle(out, rect, color,2);
            //std::ostringstream s;
            //s << i;
            //cv::putText(out, s.str(), rect.br(), cv::FONT_HERSHEY_SIMPLEX, .8, cv::Scalar(255,200,200), 1, CV_AA);

        }

    }
};

#endif // ALGORITHM_TAGGINGB_H
