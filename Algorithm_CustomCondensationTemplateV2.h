#ifndef ALGORITHM_CUSTOMCONDENSATIONTEMPLATEV2_H
#define ALGORITHM_CUSTOMCONDENSATIONTEMPLATEV2_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Algorithm_CustomCondensation.h"

#include "Matcher_TargetMatcherV2_BinaryMask.h"
#include "Target_CondensationTargetV1.h"

#include <chrono>
#include <vector>
#include <random>
#include "Algorithm_Sub_Tagging.h"

#include "Algorithm_CustomCondensationV1.h"
#include <random>
#include <iostream>
#include <map>
#include <math.h>
#include <algorithm>

template<typename M>
class CustomCondensationTemplateV2: public CustomCondensation<M>{

public:

	typedef std::chrono::high_resolution_clock Clock;
	typedef cv::Point Point;
	typedef std::vector<Point> Points;
	typedef cv::Rect Rect;
	typedef std::vector<Rect> Rects;
	typedef typename M::Target Target;
	typedef typename M::Distance Distance;
	typedef std::vector<Distance> Distances;
	typedef struct{
		Distances x;
		Distances y;
	} Distances2D;
	typedef typename Target::Score Score;
	typedef typename Target::Density Density;
	typedef typename Target::Densities Densities;
	typedef typename Target::Feature Feature;
	typedef typename Target::Features Features;
	typedef std::vector<Target> Targets;
	typedef std::discrete_distribution<int> DiscreteDistribution;
	typedef struct{
		DiscreteDistribution x;
		DiscreteDistribution y;
	} DiscreteDistributions;

	typedef std::default_random_engine Generator;
	typedef int Delta;
	typedef struct{
		Delta x, y;
	} Deltas;

	typedef struct{
		std::vector<size_t> x;
		std::vector<size_t> y;
	} RefCounter;

private:

	Generator generator = Generator(Clock::now().time_since_epoch().count());
	Targets currentlyTracked;
	size_t pollingRange = 50;
	size_t generatingRange = 1;
    int spreadRange = 5;
	//int minScore = 50;
	int TARGET_MIN_WIDTH = 50; //should be at least 1 px and correspond to rect generator (eg tagging) results for no crash
	int TARGET_MIN_HEIGHT = 50;
	int TARGET_MAX_WIDTH = 300;
	int TARGET_MAX_HEIGHT = 300;
	size_t MAX_DIST = 1000000;
	Densities density;
	Distances2D distances;
	RefCounter refCounter;

	AccumulateBackground accumulator;
	std::vector<std::vector<cv::Point> > contour;
	std::vector<cv::Vec4i> hierarchy;

	int maxCorners = 200;
	double qualityLevel = 0.01;
	double minDistance = 10;
	cv::InputArray maskArray = cv::noArray();
	int blockSize = 3;
	bool useHarrisDetector = false;
	double k = 0.04;

	cv::Mat grey;

	size_t MIN_ACCUMULATOR_ITERATIONS = 90;

	size_t accumulatorIterations = 0;


public:

	CustomCondensationTemplateV2(){}

	void process(const cv::Mat &in, cv::Mat &out){
		out = in.clone();
		cv::cvtColor(in, grey, CV_BGR2GRAY);

		accumulator.process(in);
		++accumulatorIterations;
		if(accumulatorIterations < MIN_ACCUMULATOR_ITERATIONS) return;

		for(Target& target : currentlyTracked){
			this->ConDensAte(in, out, target);
		}

		if(currentlyTracked.size() < generatingRange){
			this->pollNewTargets(in,out);
		}

		for(const Target& target : currentlyTracked){
			drawTarget(out, target);
		}

		return;
	}

private:

	Rects getMovingObjects() {

		cv::findContours(accumulator.getForeground(),
						 contour,
						 hierarchy,
						 CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );


		Rects rects;

		for(unsigned int i = 0; i< contour.size(); i++ ){
			Rect rect = cv::boundingRect(contour.at(i));
			if(rect.width > TARGET_MAX_WIDTH
					|| rect.width < TARGET_MIN_WIDTH
					|| rect.height > TARGET_MAX_HEIGHT
					|| rect.height < TARGET_MIN_HEIGHT) continue;
			rects.push_back(rect);
		}

		return rects;
	}

	void drawTarget(cv::Mat& out, const Target& target){
		if(!target.features.empty()){
			double mean[2] = {0,0};
			size_t k = 0;
			for(const Feature& feature : target.features){
				cv::circle(out, feature.point, 3, cv::Scalar(0,122,0),-1);
				mean[0] += feature.point.x;
				mean[1] += feature.point.y;
				++k;
			}

			Point center;
			center.x = mean[0]/k;
			center.y = mean[1]/k;

			cv::putText(out, "x", center, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(0,122,0));
		}
	}

	Points generatePoints(DiscreteDistributions &distributions){

		Points result;
		size_t rangeSize = (distributions.x.max() - distributions.x.min()) * (distributions.y.max() - distributions.y.min());
		for(size_t i = 0; i < pollingRange && i < rangeSize ; ++i){
			result.push_back(Point(distributions.x(generator),distributions.y(generator)));
		}
		return result;
	}

	void initDensity(const cv::Mat& in){
		density.x.clear();
		density.y.clear();
		density.x.resize(in.cols, 0);
		density.y.resize(in.rows, 0);
	}

	void initDistances(const cv::Mat& in){
		distances.x.clear();
		distances.y.clear();
		distances.x.resize(in.cols, 0);
		distances.y.resize(in.rows, 0);
	}

	void initRefCounter(const cv::Mat& in){
		refCounter.x.clear();
		refCounter.y.clear();
		refCounter.x.resize(in.cols, 0);
		refCounter.y.resize(in.rows, 0);
	}

	void ConDensAte(const cv::Mat &in, cv::Mat &out, Target& target){
		for(Feature& feature : target.features){
			this->initDensity(in);
			this->initDistances(in);
			DiscreteDistributions distributions = {
				DiscreteDistribution(feature.density.x.begin(), feature.density.x.end()),
				DiscreteDistribution(feature.density.y.begin(), feature.density.y.end())
			};

			Points points = generatePoints(distributions);

			std::map<Score, cv::Point> scores;
			this->initRefCounter(in);

			for(const Point& point : points){

				Distance distance = this->matcher.computeDistance(grey, feature.point, point);
				++refCounter.x[point.x];
				++refCounter.y[point.y];
				distances.x[point.x] += distance;
				distances.y[point.y] += distance;

				scores.insert(std::pair<Score, cv::Point>(distance, point));

                //cv::circle(out, point, 3, cv::Scalar(0,0,255),-1);
			}

			for(size_t i = 0; i < refCounter.x.size(); ++i){
				if(refCounter.x[i] > 1){
					distances.x[i] /= refCounter.x[i];
				}
			}

			for(size_t i = 0; i < refCounter.y.size(); ++i){
				if(refCounter.y[i] > 1){
					distances.y[i] /= refCounter.y[i];
				}
			}

			for(size_t i = 0; i < distances.x.size(); ++i){
				if(refCounter.x[i] > 0){
					density.x[i] = (distances.x[i] >= MAX_DIST)? 0 : MAX_DIST - distances.x[i];
				}
			}

			for(size_t i = 0; i < distances.y.size(); ++i){
				if(refCounter.y[i] > 0){
					density.y[i] = (distances.y[i] >= MAX_DIST)? 0 : MAX_DIST - distances.y[i];
				}
			}


			Deltas deltas = this->shift(feature.density);
			this->spread(deltas);

			feature.density = density;
			feature.point = scores.begin()->second;
		}
	}

	void pollNewTargets(const cv::Mat &in, cv::Mat &out){


		Rects rects = getMovingObjects();

		cv::Scalar color = cv::Scalar(0,0x80,0);

		Deltas deltas;
		deltas.x = 0;
		deltas.y = 0;
		for(const Rect& rect : rects){
			std::vector<cv::Point2f> corners;
			cv::goodFeaturesToTrack(grey(rect), corners,
									maxCorners, qualityLevel,
									minDistance, maskArray,
									blockSize, useHarrisDetector, k);
			Target target;
			target.features.reserve(corners.size());
			for(Point corner : corners){
				corner.x += rect.x;
				corner.y += rect.y;
				this->initDensity(in);
				density.x[corner.x] = MAX_DIST;
				density.y[corner.y] = MAX_DIST;
				spread(deltas);
				Feature feature;
				feature.point = corner;
				feature.density = density;
				target.features.push_back(feature);
				cv::circle(out, feature.point, 3, color,-1);
			}


			currentlyTracked.push_back(target);
			std::cout << "pushed new target" << std::endl;
			cv::rectangle(out, rect, color);
		}
	}

	Deltas shift(const Densities& prevDensity){
		Deltas deltas;
		deltas.x = shift(density.x, prevDensity.x);
		deltas.y = shift(density.y, prevDensity.y);
		return deltas;
	}

	void spread(Deltas deltas){
		spread(density.x, deltas.x, spreadRange);
		spread(density.y, deltas.y, spreadRange);
	}

	Delta shift(Density& density, const Density& prevDensity){

		std::pair<size_t, int> prevMax(0,~0);
		size_t j = 0;
		for(int i : prevDensity){
			if(prevMax.second < i){
				prevMax.first = j;
				prevMax.second = i;
			}
			++j;
		}

		std::pair<size_t, int> max(0,~0);
		j = 0;
		for(int i : density){
			if(max.second < i){
				max.first = j;
				max.second = i;
			}
			++j;
		}

		Delta delta = max.first > prevMax.first ? max.first - prevMax.first : -(prevMax.first - max.first);

		if(delta != 0){
			Density temp;
			if(delta > 0){
				temp.resize(delta, 0);
				temp.insert(temp.end(), density.begin(), density.end()-delta);
			}
			else{
				temp.assign(density.begin()-delta, density.end());
				temp.resize(density.size(), 0);
			}
		}

		return delta;
	}

	void spread(Density& density, Delta delta, int spreadRange){
		Density temp = density;
		size_t blurRange = abs(delta) + spreadRange;
		int size = density.size();
		for(size_t d = 0; d < blurRange; ++d){
			for(int i = 0; i < size; ++i){
				int val = 0;
				int k = 0;
				int begin = std::max(0,i-1);
				int end = std::min(size,i+2);
				for(int j = begin; j < end; ++j, ++k){
					val += temp[j];
				}
				if(k > 0) density[i] = val/k;
			}
			temp = density;
		}
	}



};
#endif // ALGORITHM_CUSTOMCONDENSATIONTEMPLATEV2_H
