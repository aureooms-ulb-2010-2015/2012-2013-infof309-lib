#ifndef ALGORITHM_SUB_TAGGINGB_H
#define ALGORITHM_SUB_TAGGINGB_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <list>
#include <algorithm>

class Sub_TaggingB{

public:
    typedef std::vector<cv::Rect> Rects;
    typedef std::list<cv::Rect> RectsList;

private:

    cv::Mat _gray;			// current gray-level image
    cv::Mat _background;	// accumulated background
    cv::Mat _backImage;		// background image
    cv::Mat _foreground;		// foreground image

    std::vector<std::vector<cv::Point> > _contour;
    std::vector<cv::Vec4i> _hierarchy;
    cv::Rect _bndRect;

    int _threshold;			// threshold for foreground extraction
    double _learningRate;    // learning rate in background accumulation
    double _globalMeanArea, _currentMeanArea;
    unsigned int _processCounter;


    cv::Size _blurRange;

public:

    Sub_TaggingB() : _threshold(10), _learningRate(0.1), _globalMeanArea(0.), _processCounter(0), _blurRange(10,10){
        std::cout<<"sub tagging b"<<std::endl;
    }

    // processing method
    Rects process(const cv::Mat &in) {
        _currentMeanArea = (_processCounter-1 == 0)? 0. : _globalMeanArea/_processCounter;
        _processCounter+=1;

        // convert input frame to gray-level image and blur it
        cv::cvtColor(in, _gray, CV_BGR2GRAY);
        cv::blur( _gray, _gray, _blurRange );


        // initialize background to 1st frame
        if (_background.empty()){
            _gray.convertTo(_background, CV_32F);// 32 bit floating point
        }

        // convert background to unsigned 8bit/pixel (values 0-255)
        _background.convertTo(_backImage,CV_8U);


        // compute difference between current image and background
        cv::absdiff(_backImage,_gray,_foreground);

        // contrast the image
        cv::threshold(_foreground,_foreground,12,255,cv::THRESH_BINARY_INV);
        //CV_RETR_CCOMP
        //foreground = 255 - foreground;
        int erosion_type = cv::MORPH_RECT;
        int erosion_size = 1;


        cv::Mat element = cv::getStructuringElement( erosion_type, cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ), cv::Point( erosion_size, erosion_size ) );
        cv::morphologyEx(_foreground,_foreground,cv::MORPH_OPEN,element);
        cv::erode(_foreground,_foreground,cv::Mat());
        cv::dilate(_foreground,_foreground,cv::Mat());

        cv::findContours(_foreground, _contour, _hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );


        const double GROUP_EPS = 1.;
        Rects rects;


        double localMeanArea = 0.;
        for(unsigned int i = 5; i< _contour.size(); i++ ){
                double area = cv::contourArea(_contour.at(i));
                if( fabs(area)>=fabs(_currentMeanArea/2.) && fabs(area) <= (in.cols*in.rows)/4 ){
                    rects.push_back(cv::boundingRect(_contour.at(i)));
                    cv::groupRectangles(rects,0,GROUP_EPS);
                    localMeanArea += fabs(area/_contour.size());

                }

        }

        std::sort(rects.begin(), rects.end(), Sub_TaggingB::compareRect);
        _globalMeanArea +=localMeanArea;

        rects = cleanRects(rects);
        cv::groupRectangles(rects,3,GROUP_EPS);


        cv::Mat temp;
        // apply threshold to foreground image
        cv::threshold(_foreground,temp,_threshold,255,cv::THRESH_BINARY_INV);
        // accumulate background
        cv::accumulateWeighted(_gray, _background, _learningRate, temp);

        return rects;
    }

bool closeTo(const cv::Rect &a, const cv::Rect &b, const int delta){
    bool res;
    cv::Point a1, b1, actr, bctr, centerDist;
    actr = centerRect(a);
    bctr = centerRect(b);

    centerDist = absDistance(actr,bctr);
    a1 = absDistance(actr,a.tl());
    b1 = absDistance(bctr,b.tl());
    int seuilX = (a1.x+b1.x+delta);
    int seuilY = (a1.y+b1.y+delta);
    res = (centerDist.x<=seuilX) && (centerDist.y <= seuilY);
    return res;

}

cv::Point absDistance(const cv::Point a, const cv::Point b){
    cv::Point absDist;
    absDist.x = abs(a.x-b.x);
    absDist.y = abs(a.y-b.y);
    return absDist;
}


cv::Point centerRect(const cv::Rect &rect){
    cv::Point center;
    center.x = rect.x+rect.width/2;
    center.y = rect.y+rect.height/2;
    return center;
}


cv::Rect extend(const cv::Rect &o, const cv::Rect &s){

    cv::Point lt = cv::Point(std::min(o.tl().x,s.tl().x),std::min(o.tl().y,s.tl().y));
    cv::Point br = cv::Point(std::max(o.br().x,s.br().x),std::max(o.br().y,s.br().y));
    cv::Rect res = cv::Rect(lt,br);
    return res;
}


cv::Rect merge(const cv::Rect &o, const cv::Rect &s){
    cv::Rect res;
    if (closeTo(o,s,25) ) {
        res = extend(o,s);
    }
    else{
        res = o;
    }
    return res;
}


Rects cleanRects(Rects &rects){
    Rects rectsVect = rects;
    Rects subRectsVect;


    for (Rects::iterator ita=rectsVect.begin(); ita != rectsVect.end(); ++ita){
        cv::Rect res;
        for (Rects::iterator itb=rectsVect.begin(); itb != rectsVect.end(); ++itb){
            if(*ita!=*itb) {
                cv::Rect temp = merge(*ita,*itb);
                res = (abs(temp.area())>abs(res.area()))? temp:res;
            }
        }
        subRectsVect.push_back(res);
    }
    rectsVect = subRectsVect;
    return rectsVect;

}

static bool compareRect(cv::Rect a, cv::Rect b){
    return fabs(a.area())<fabs(b.area());
}



};
#endif // ALGORITHM_SUB_TAGGINGB_H
