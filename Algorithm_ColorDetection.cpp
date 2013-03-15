#include "Algorithm_ColorDetection.h"

ColorDetection::ColorDetection(){}

void ColorDetection::process(const cv::Mat &in, cv::Mat &out) {
    std::vector<cv::Vec3f> vecCircles;
    std::vector<cv::Vec3f>::iterator iterCircles;
    cv::GaussianBlur(in, out, cv::Size(9,9),1.5);

    //cv::Scalar(BGR)
    cv::inRange(out, cv::Scalar(0,0,175), cv::Scalar(100,100,255), out);
    cv::GaussianBlur(out, out, cv::Size(9,9),1.5);
    cv::HoughCircles(out, vecCircles, CV_HOUGH_GRADIENT, 2, out.rows/4, 100, 50, 10, 400);
    out = in.clone();
    cv::Point redPoint;
    for(iterCircles = vecCircles.begin(); iterCircles != vecCircles.end(); ++iterCircles) {
        cv::circle(out, cv::Point((int)(*iterCircles)[0], (int)(*iterCircles)[1]), 3, cv::Scalar(255,255,255), CV_FILLED);
        cv::circle(out, cv::Point((int)(*iterCircles)[0], (int)(*iterCircles)[1]), (int)(*iterCircles)[2], cv::Scalar(0,0,255),3);
        redPoint = cv::Point((int)(*iterCircles)[0], (int)(*iterCircles)[1]);
    }
    cv::putText(out, "Red", redPoint, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(0,0,255), 1, CV_AA);



    return;







}
