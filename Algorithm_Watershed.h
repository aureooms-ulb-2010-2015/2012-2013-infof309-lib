#ifndef ALGORITHM_WATERSHED_H
#define ALGORITHM_WATERSHED_H

#include "FrameProcessor.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class Watershed : public FrameProcessor {

private:
    cv::Mat _gray;
    cv::Mat _mask;
    cv::Mat _bckgrnd;
    cv::Mat _bckgrndImage;
    cv::Mat _bg;
    cv::Mat _bg1;
    cv::Mat _foregrnd;
    cv::Mat _marker;
    cv::Mat _marker32;
    cv::Mat _m;
    cv::Mat _rect;
    int _mode;

    // contour
    std::vector<std::vector<cv::Point> > contour;
    std::vector<cv::Vec4i> hierarchy;
    cv::Rect _bndRect;
    cv::RNG rng;


public:
    Watershed(const int mode = 0){
        _mode = mode;
    }


    void process(const cv::Mat &input, cv::Mat &output){
        // convert input frame to gray-level image and blur it
        cv::cvtColor(input, _gray, CV_BGR2GRAY);
        cv::blur(_gray, _gray, cv::Size(16,16) );
        if(_bckgrnd.empty()){
            _gray.convertTo(_bckgrnd,CV_32F);
            _gray.convertTo(_mask, CV_8U);
        }
        _bckgrnd.convertTo(_bckgrndImage, CV_8U);

        /*
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
*/
        cv::threshold(_mask,_mask,0,255,CV_THRESH_BINARY+CV_THRESH_OTSU);
        cv::absdiff(_bckgrndImage,_gray,_mask);


        //foreground sure
        cv::erode(_mask,_foregrnd,cv::Mat(),cv::Point(-1,-1),3);
        //background sure
        cv::dilate(_mask,_bg,cv::Mat(),cv::Point(-1,-1),4);

        //marker create
        //fusion bg & fg as marker
        cv::threshold(_bg,_bg,1,128,CV_THRESH_BINARY_INV+CV_THRESH_OTSU);
        cv::add(_foregrnd,_bg,_marker);

        _marker.convertTo(_marker32,CV_32SC1);
        cv::watershed(input,_marker32);
        cv::convertScaleAbs(_marker32,_m);
        cv::threshold(_m,_mask,0,255,CV_THRESH_BINARY+CV_THRESH_OTSU);

        /*test
        int erosion_type = cv::MORPH_ELLIPSE;
        int erosion_size = 1;
        cv::Mat element = cv::getStructuringElement( erosion_type, cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ), cv::Point( erosion_size, erosion_size ) );
        cv::dilate(_mask,_mask,element,cv::Point(-1,-1),4);*/

        int erosion_type = cv::MORPH_RECT;
        int erosion_size = 8;
        cv::Mat element = cv::getStructuringElement( erosion_type, cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ), cv::Point( -1, -1 ) );
        cv::morphologyEx(_mask,_mask,cv::MORPH_CLOSE,element);


        cv::accumulateWeighted(_gray, _bckgrnd,0.01, output);
        if(output.empty()){
            output=input.clone();
            _rect = input.clone();
            _rect.setTo(cv::Scalar(255,255,255));
            if(_mode>0){
                output.setTo(cv::Scalar(255,255,255));// for white foreground
            }
        }
        output.setTo(cv::Scalar(0,0,0),_mask);

        //contour tagging
        cv::findContours(_mask, contour, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
        cv::Scalar color = cv::Scalar(123,160,142);

        //Get & Draw the bounding rectangle around the moving object
        for(unsigned int i = 0; i< contour.size(); i++ ){
            double area = cv::contourArea(contour.at(i));
            _bndRect = cv::boundingRect(contour.at(i));
            //cv::drawContours(_mask,contour,i,cv::Scalar(255,255,255),CV_FILLED);

            if( fabs(area) >= 1000){
                //cv::rectangle(_mask, _bndRect, cv::Scalar(255,255,255), CV_FILLED,8,0 );
                cv::rectangle(output, _bndRect, color, 1,8,0 );
            }else{
                cv::drawContours(_rect,contour,i,cv::Scalar(0,0,0),CV_FILLED);
            }
        }
        cv::bitwise_and(output,_rect,output);
        //_mask.convertTo(output,CV_32F);
    }
};

#endif // ALGORITHM_WATERSHED_H
