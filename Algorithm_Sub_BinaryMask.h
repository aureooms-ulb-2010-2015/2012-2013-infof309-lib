#ifndef ALGORITHM_SUB_BINARYMASK_H
#define ALGORITHM_SUB_BINARYMASK_H

#include "FrameProcessor.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class Sub_BinaryMask : public FrameProcessor {

private:
    cv::Mat _gray;
    cv::Mat _mask;
    cv::Mat _bckgrnd;
    cv::Mat _bckgrndImage;


public:
    Sub_BinaryMask(){}


    void process(const cv::Mat &input, cv::Mat &output){
        // convert input frame to gray-level image and blur it
        cv::cvtColor(input, _gray, CV_BGR2GRAY);
        cv::blur(_gray, _gray, cv::Size(16,16) );
        if(_bckgrnd.empty()){
            _gray.convertTo(_bckgrnd,CV_32F);
            _gray.convertTo(_mask, CV_8U);
        }
        _bckgrnd.convertTo(_bckgrndImage, CV_8U);

        cv::threshold(_mask,_mask,12,255,CV_THRESH_BINARY);
        cv::erode(_mask,_mask,cv::Mat());
        cv::dilate(_mask,_mask,cv::Mat());
        cv::absdiff(_bckgrndImage,_gray,_mask);
        cv::threshold(_mask,_mask,12,255,CV_THRESH_BINARY);

        //cv::adaptiveThreshold(_mask,_mask,255,,CV_THRESH_BINARY_INV,6,1)
        int erosion_type = cv::MORPH_RECT;
        int erosion_size = 1;
        cv::Mat element = cv::getStructuringElement( erosion_type, cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ), cv::Point( erosion_size, erosion_size ) );
        cv::morphologyEx(_mask,_mask,cv::MORPH_OPEN,element);

        //cv::blur(_mask,_mask,cv::Size(5,5));
        /*
        int erosion_type = cv::MORPH_ELLIPSE;
        int erosion_size = 0;
        cv::Mat element = cv::getStructuringElement( erosion_type, cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ), cv::Point( erosion_size, erosion_size ) );
        cv::dilate(_mask,_mask,element,cv::Point(-1,-1),4);
        */

            //_mask.convertTo(_out, CV_32F);

        //_gray.copyTo(output,_mask);
        cv::accumulateWeighted(_gray, _bckgrnd,0.1, output);
        if(output.empty()){
        output=input.clone();
        //output.create(input.rows,input.cols,input.type());
        output.setTo(cv::Scalar(255,255,255));
        }
        output.setTo(cv::Scalar(0,0,0),_mask);
        //output.copyTo(output,_mask);
        //input.copyTo(output,_mask);
    }
};

#endif // ALGORITHM_SUB_BINARYMASK_H
