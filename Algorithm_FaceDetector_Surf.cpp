#include "Algorithm_FaceDetector_Surf.h"
#include "RobustMatcher.h"
#include <opencv2/objdetect/objdetect.hpp>
#include <string>
#include <ostream>
#include <iostream>

FaceDetector_Surf::FaceDetector_Surf() {
    std::string filename = "lpbcascade_frontalface.xml";
    this->_corner.x=0;
    this->_corner.y=0;
	if (!this->_classifier.load(filename)) std::cout << "can't load file : " << filename << std::endl;
}


bool recognize(const cv::Mat& , cv::Rect);


void FaceDetector_Surf::process(const cv::Mat &in, cv::Mat &out) {
    // Create a new image based on the input image
    out = in.clone();

    // There can be more than one face in an image
    std::vector<cv::Rect> faces;

    // Detect the objects and store them in the sequence
    this->_classifier.detectMultiScale(out, faces, 1.2, 3, CV_HAAR_DO_CANNY_PRUNING, cv::Size(25, 25));

    // Loop the number of faces found.
    for( size_t i = 0; i < faces.size(); ++i )  {
        // Draw the rectangle in the input image
        cv::rectangle( out, faces.at(i), cv::Scalar(255,0,0), 3, 8, 0);
    }

    cv::Point point;
    point.x=20; point.y=20;

    std::ostringstream faces_;
    faces_ << "Number of Faces : " << faces.size();

    cv::putText(out, faces_.str(), point, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(255,0,0), 1, CV_AA);

    try {
        if (faces.size()>0){
            bool recon = recognize(in, faces.at(0));
            std::string name="";
            cv::Point label;
            label.x=((faces.at(0)).br()).x+10;
            label.y=((faces.at(0)).br()).y+10;
            if (recon) {
                name ="Simon";
            }else {
                name ="John Doe";
            }
            cv::putText(out, name, label, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(255,0,0), 1, CV_AA);
            std::cout << "\n corner x avant :"<< this->getCorner().x ;
            std::cout << " | corner x apres :"<< faces.at(0).br().x << std::endl;
            this->setCorner((faces.at(0)).br());
        }
    } catch(std::exception e){}

    return;
}



bool recognize(const cv::Mat &in, cv::Rect roi) {

    bool result= false;
    cv::Mat image1 = cv::imread("sim.png",0);

    // Prepare the matcher
    RobustMatcher rmatcher;
    rmatcher.setConfidenceLevel(0.98);
    rmatcher.setMinDistanceToEpipolar(1.0);
    rmatcher.setRatio(0.65f);

    cv::Ptr<cv::FeatureDetector> pfd = new cv::SurfFeatureDetector(10);
    rmatcher.setFeatureDetector(pfd);

    cv::Mat face = in(roi);


    // Match the two images
    std::vector<cv::DMatch> matches;
    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    rmatcher.match(image1, face ,matches, keypoints1, keypoints2);


    // draw the matches
    cv::Mat imageMatches;
    cv::drawMatches(image1,keypoints1,          // 1st image and its keypoints
                    face,keypoints2,            // 2nd image and its keypoints
                    matches,                    // the matches
                    imageMatches,               // the image produced
                    cv::Scalar(255,255,255));   // color of the lines


    if (rmatcher.getRecognition()){
        result=true;
    }

    return result;
}
