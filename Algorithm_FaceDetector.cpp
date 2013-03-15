#include "Algorithm_FaceDetector.h"
#include <opencv2/objdetect/objdetect.hpp>
#include <string>
#include <ostream>

FaceDetector::FaceDetector(){
	this->_classifier.load("haarcascade_frontalface_default.xml");
}

//void recognize(const cv::Mat &in);

void FaceDetector::process(const cv::Mat &in, cv::Mat &out) {

    // Create a new image based on the input image
    out = in.clone();
    std::vector<cv::Rect> faces;


    // There can be more than one face in an image.
    // So create a growable sequence of faces.
	// Detect the objects and store them in the sequence
	this->_classifier.detectMultiScale(out, faces, 1.2, 3, CV_HAAR_DO_CANNY_PRUNING, cv::Size(25, 25));
    int nbFaces=0;

	// Loop the number of faces found.
	for( size_t i = 0; i < faces.size(); ++i )  {
        // Draw the rectangle in the input image
        cv::rectangle( out, faces.at(i), cv::Scalar(255,0,0), 3, 8, 0);
    }
    cv::Point point;
    point.x=20;
    point.y=20;


    std::string faces_ = static_cast<std::ostringstream*>( &(std::ostringstream() << nbFaces) )->str();

    //cv::Mat test;
    cv::putText(out, "Number of Faces : "+faces_, point, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(255,0,0), 1, CV_AA);
    return;

}


