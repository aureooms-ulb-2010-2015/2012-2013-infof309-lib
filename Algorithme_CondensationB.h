#ifndef ALGORITHM_CondensationB_H
#define ALGORITHM_CondensationB_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/video/tracking.hpp>
#include <random>
#include <cmath>
#include <chrono>
#include <opencv2/ml>


class CondensationB: public CondensationB{

public:

    typedef std::chrono::high_resolution_clock Clock;
    typedef std::vector<cv::Point2f> Features;
    typedef std::vector<double> Weights;
    typedef struct{
        double estimation;
        unsigned int nParticles;
        Features particles;
        Weights weights;
    } State;

    typedef std::default_random_engine Generator;
    typedef std::normal_distribution<double> Distribution;


private:

    cv::Mat gray;			// current gray-level image
    cv::Mat gray_prev;		// previous gray-level image

    // global system state
    State predictState;
    State observState;

    //covariance
    double Rx;
    double Qx;

    // Luminance Invariant
    Features xLinearPrev;
    Features xLinearPredict;
    Features xLinearObserv;
    Weights xLinearWeights;

    // config goodFeatureToTrack for linear feature detection or Harris corner
    /**
    void goodFeaturesToTrack(InputArray image, OutputArray corners, int maxCorners, double qualityLevel,
    double minDistance, InputArray mask=noArray(), int blockSize=3, bool useHarrisDetector=false, double k=0.04 )
    **/
    int maxCorners = 23;    // Maximum number of corners to return.
    double qualityLevel = 0.01; // Minimal accepted quality of image corners.
    double minDistance = 10; // Minimum possible Euclidean distance between the returned corners.
    int blockSize = 3;  // Size of an average block for computing a derivative covariation matrix
    bool useHarrisDetector = false;
    double k = 0.04;// Free parameter of the Harris detector


    //OFC (optical flow constraint)
    Features xOfcPrev;
    Features xOfcObserv;
    Features xOfcPredict;
    Weights xOfcWeights;

    /**
    void calcOpticalFlowPyrLK( InputArray prevImg, InputArray nextImg, InputArray prevPts,
    InputOutputArray nextPts, OutputArray status, OutputArray err, Size winSize=Size(21,21),
    int maxLevel=3, TermCriteria criteria=TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01),
    int flags=0, double minEigThreshold=1e-4 )
    **/
    std::vector<uchar> status; // status of tracked features
    std::vector<float> err;    // error in tracking

public:

    CondensationB();
    void process(const cv::Mat &in, cv::Mat &out){
        // convert to gray-level image
        cv::cvtColor(in, gray, CV_BGR2GRAY);

    }

    void init(){ // initialize
        //cv::calcCovarMatrix();
        //cv::Mahalanobis();

    }

    void observe(){ // actualize observation
        for (int i = 0; i < predictState.nParticles; ++i) {
            cv::goodFeaturesToTrack(gray,xLinearObserv[i],maxCorners,qualityLevel,minDistance);
            cv::calcOpticalFlowPyrLK(gray_prev,gray,xOfcPrev[i],xOfcObserv[i],status,err);
        }
    }

    void measure(){ // weight possible trajectoire
        // mesurer distance euclidienne entre points(Lineaire et OFC)
        for (int i = 0; i < predictState.nParticles; ++i) {
            xLinearWeights[i] = hypothenus(xLinearPredict[i],xLinearObserv[i]);
            xOfcWeights[i] = hypothenus(xOfcPredict[i],xOfcObserv[i]);
        }
    }

    void predict(){ // propagate N particule
        for (int i = 0; i < predictState.nParticles; ++i) {
            cv::Point2f x0 = xLinearPrev[i];
            cv::Point2f xk = xLinearObserv[i];
            cv::Point2f prediction;
            prediction.x = linearPredict(x0.x,xk.x);
            prediction.y = linearPredict(x0.y,xk.y);
            xLinearPredict[i] = prediction;
        }
    }

    float linearPredict(float x0, float xk){
        // X_i(k+1) = f_x(X_i(k-1),X_i(k))
        double wk = gaussian(0,Rx);
        double vk = gaussian(0,Qx);
        return (xk-x0)*vk + xk + wk;
    }

    void likelihoodValuation(){ // estimate Global State

    }

    void ReSampling(){ // reSample N particles

    }

    void normalize(){ // Normalize weight

    }

    //utils

    unsigned int hypothenus(const cv::Point a, const cv::Point b){
        return std::sqrt(std::pow(a.x-b.x,2)+std::pow(a.y-b.y,2));
    }

    double gaussian(float mu, float sigma){
        Generator g;
        Distribution d(mu,sigma);
        return d(g);
    }

    double update(double muX, double muZ , double sigmaZ){
        static const double PI = 3.14159265358979323846;
        return 1.0/(std::sqrt(2.0*PI) * sigmaZ) * std::exp(-0.5 * ( (muZ-muX) / (sigmaZ*sigmaZ)));
    }

    //derivating: fprime(x) = (f(x+dx) - f(x-dx)) / (2*dx)




};

#endif // ALGORITHM_CondensationB_H
