#ifndef ROBUSTMATCHER_H
#define ROBUSTMATCHER_H

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/legacy/legacy.hpp>

class RobustMatcher {

private:
    // Pointer to the feature point detector object
    cv::Ptr<cv::FeatureDetector> _detector;

    // pointer to the feature descriptor extractor object
    cv::Ptr<cv::DescriptorExtractor> _extractor;

    float _ratio;       // max ratio between 1st and 2nd NN
    bool _refineF;      // if true will refine the F matrix
	double _confidence; // confidence level (probability)
	double _distance;   // min distance to epipolar
    bool _reconized;


public:
    RobustMatcher();

    // Set the feature detector
    void setFeatureDetector(cv::Ptr<cv::FeatureDetector>& detect) {
        this->_detector = detect;
    }

    // Set descriptor extractor
    void setDescriptorExtractor(cv::Ptr<cv::DescriptorExtractor>& desc) {
        this->_extractor = desc;
    }

    // Set the minimum distance to epipolar in RANSAC
    void setMinDistanceToEpipolar(double d) {
        this->_distance = d;
    }

    // Set confidence level in RANSAC
    void setConfidenceLevel(double c) {
        this->_confidence = c;
    }

    // Set the NN ratio
    void setRatio(float r) {
        this->_ratio = r;
    }

    // if you want the F matrix to be recalculated
    void refineFundamental(bool flag) {
        this->_refineF = flag;
    }

    bool getRecognition() {
      return this->_reconized;
    }

    int ratioTest(std::vector<std::vector<cv::DMatch> >& matches);

    void symmetryTest(const std::vector<std::vector<cv::DMatch> >& matches1,
        const std::vector<std::vector<cv::DMatch> >& matches2, std::vector<cv::DMatch>& symMatches);

	cv::Mat ransacTest(const std::vector<cv::DMatch>& matches,
        const std::vector<cv::KeyPoint>& keypoints1,
		const std::vector<cv::KeyPoint>& keypoints2,std::vector<cv::DMatch>& outMatches);

	cv::Mat match(cv::Mat& image1, cv::Mat& image2,
		std::vector<cv::DMatch>& matches,
		std::vector<cv::KeyPoint>& keypoints1, std::vector<cv::KeyPoint>& keypoints2);

};

#endif // ROBUSTMATCHER_H
