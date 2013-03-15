#include "Algorithm_EyeFaceDetector.h"
#include <opencv2/legacy/blobtrack.hpp>


EyeFaceDetector::EyeFaceDetector() {
    this->_classifier.load("haarcascade_frontalface_default.xml");
    this->_eyeClassifier.load("haarcascade_eye_tree_eyeglasses.xml");
}


void EyeFaceDetector::process(const cv::Mat &in, cv::Mat &out) {

    out = in.clone();
    int i = 0;
    double t = 0;
    double scale = 3.0;
    std::vector<cv::Rect> faces;
    const static cv::Scalar colors[] =  {
        CV_RGB(0,0,255),   CV_RGB(0,128,255),
        CV_RGB(0,255,255), CV_RGB(0,255,0),
        CV_RGB(255,128,0), CV_RGB(255,255,0),
        CV_RGB(255,0,0),   CV_RGB(255,0,255)} ;

    this->_classifier.detectMultiScale(out, faces, 1.2, 3, CV_HAAR_DO_CANNY_PRUNING, cv::Size(25, 25));

    cv::Mat gray, smallImg( cvRound (in.rows/scale), cvRound(in.cols/scale), CV_8UC1 );


    cvtColor( in, gray, CV_BGR2GRAY );
    resize( gray, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR );
    equalizeHist( smallImg, smallImg );
    t = (double)cvGetTickCount();
    this->_classifier.detectMultiScale( smallImg, faces,
                    1.1, 2, 0
                    //|CV_HAAR_FIND_BIGGEST_OBJECT,
                    //|CV_HAAR_DO_ROUGH_SEARCH,
                    |CV_HAAR_SCALE_IMAGE,
                    cv::Size(30, 30) );
                t = (double)cvGetTickCount() - t;


    //Draw Circle on the output image
    for( std::vector<cv::Rect>::const_iterator r = faces.begin(); r != faces.end(); r++, i++ ) {
        cv::Mat smallImgROI;
        std::vector<cv::Rect> nestedObjects;
        cv::Point center;
        cv::Scalar color = colors[i%8];
        int radius;
        center.x = cvRound((r->x + r->width*0.5)*scale);
        center.y = cvRound((r->y + r->height*0.5)*scale);
        radius = cvRound((r->width + r->height)*0.25*scale);
        cv::circle( out, center, radius, color, 3, 8, 0 );
        cv::Point label;
        label.x=center.x+radius+10;
        label.y=center.y;
        cv::putText(out, "Face detected", label, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, color, 1, CV_AA);
        //  Comment eye detection to make it faster...
        //detecting eye
        if( this->_eyeClassifier.empty() ) {
            std::cerr<<"error";
            continue;
        }
        smallImgROI = smallImg(*r);
        this->_eyeClassifier.detectMultiScale( smallImgROI, nestedObjects,
                        1.1, 2, 0
                        //|CV_HAAR_FIND_BIGGEST_OBJECT,
                        //|CV_HAAR_DO_ROUGH_SEARCH,
                        |CV_HAAR_DO_CANNY_PRUNING,
                        //|CV_HAAR_SCALE_IMAGE,
                        cv::Size(30, 30) );


        for( std::vector<cv::Rect>::const_iterator nr = nestedObjects.begin(); nr != nestedObjects.end(); nr++ ) {
            center.x = cvRound((r->x + nr->x + nr->width*0.5)*scale);
            center.y = cvRound((r->y + nr->y + nr->height*0.5)*scale);
            radius = cvRound((nr->width + nr->height)*0.25*scale);
            cv::circle( out, center, radius, color, 3, 8, 0 );
        }
    }


}


/*

bool EyeFaceDetector::recognize(cv::Mat img1, cv::Mat img2) {



    // Read input images
    cv::Mat image1= cv::imread("./church01.jpg",0);
    cv::Mat image2= cv::imread("./church02.jpg",0);
    if (!image1.data || !image2.data)
        return 0;

    // Display the images
    cv::namedWindow("Right Image");
    cv::imshow("Right Image",image1);
    cv::namedWindow("Left Image");
    cv::imshow("Left Image",image2);

    // Prepare the matcher
    RobustMatcher rmatcher;
    rmatcher.setConfidenceLevel(0.98);
    rmatcher.setMinDistanceToEpipolar(1.0);
    rmatcher.setRatio(0.65f);
    cv::Ptr<cv::FeatureDetector> pfd= new cv::SurfFeatureDetector(10);
    rmatcher.setFeatureDetector(pfd);

    // Match the two images
    std::vector<cv::DMatch> matches;
    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat fundemental= rmatcher.match(image1,image2,matches, keypoints1, keypoints2);

    // draw the matches
    cv::Mat imageMatches;
    cv::drawMatches(image1,keypoints1,  // 1st image and its keypoints
                    image2,keypoints2,  // 2nd image and its keypoints
                    matches,			// the matches
                    imageMatches,		// the image produced
                    cv::Scalar(255,255,255)); // color of the lines
    cv::namedWindow("Matches");
    cv::imshow("Matches",imageMatches);

    // Convert keypoints into Point2f
    std::vector<cv::Point2f> points1, points2;

    for (std::vector<cv::DMatch>::const_iterator it= matches.begin();
             it!= matches.end(); ++it) {

             // Get the position of left keypoints
             float x= keypoints1[it->queryIdx].pt.x;
             float y= keypoints1[it->queryIdx].pt.y;
             points1.push_back(cv::Point2f(x,y));
             cv::circle(image1,cv::Point(x,y),3,cv::Scalar(255,255,255),3);
             // Get the position of right keypoints
             x= keypoints2[it->trainIdx].pt.x;
             y= keypoints2[it->trainIdx].pt.y;
             cv::circle(image2,cv::Point(x,y),3,cv::Scalar(255,255,255),3);
             points2.push_back(cv::Point2f(x,y));
    }

    // Draw the epipolar lines
    std::vector<cv::Vec3f> lines1;
    cv::computeCorrespondEpilines(cv::Mat(points1),1,fundemental,lines1);

    for (vector<cv::Vec3f>::const_iterator it= lines1.begin();
             it!=lines1.end(); ++it) {

             cv::line(image2,cv::Point(0,-(*it)[2]/(*it)[1]),
                             cv::Point(image2.cols,-((*it)[2]+(*it)[0]*image2.cols)/(*it)[1]),
                             cv::Scalar(255,255,255));
    }

    std::vector<cv::Vec3f> lines2;
    cv::computeCorrespondEpilines(cv::Mat(points2),2,fundemental,lines2);

    for (vector<cv::Vec3f>::const_iterator it= lines2.begin();
             it!=lines2.end(); ++it) {

             cv::line(image1,cv::Point(0,-(*it)[2]/(*it)[1]),
                             cv::Point(image1.cols,-((*it)[2]+(*it)[0]*image1.cols)/(*it)[1]),
                             cv::Scalar(255,255,255));
    }

    // Display the images with epipolar lines
    cv::namedWindow("Right Image Epilines (RANSAC)");
    cv::imshow("Right Image Epilines (RANSAC)",image1);
    cv::namedWindow("Left Image Epilines (RANSAC)");
    cv::imshow("Left Image Epilines (RANSAC)",image2);

    cv::waitKey();
    return 0;
    }




*/











