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
#include "CondensationParameters.h"

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

	typedef struct{
		int mean[2] = {0,0};
		int deviation[2] = {0,0};
	} Gaussian;

	typedef std::vector<cv::Point> Contour;
	typedef std::vector<Contour> Contours;

private:

	CondensationParameters parameters;

	cv::Scalar COLOR_NEW_FEATURE =  cv::Scalar(0x98, 0x59, 0x3b);
	cv::Scalar COLOR_WHITE = cv::Scalar::all(255);
	cv::Scalar COLOR_BLACK = cv::Scalar(0,0,0);

	Generator generator = Generator(Clock::now().time_since_epoch().count());
	Targets currentlyTracked;
	Densities density;
	Distances2D distances;
	RefCounter refCounter;

	AccumulateBackground accumulator;
	Contours contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::InputArray maskArray = cv::noArray();

	cv::Mat grey;
	cv::Mat grey_prev;


	size_t accumulatorIterations = 0;


public:

	CustomCondensationTemplateV2(){}

	void process(const cv::Mat &in, cv::Mat &out){
		out = in.clone();

		accumulator.process(in);
		++accumulatorIterations;
		if(accumulatorIterations < parameters.MIN_ACCUMULATOR_ITERATIONS) return;
		cv::cvtColor(in, grey, CV_BGR2GRAY);

		if(!grey_prev.empty()){
			for(Target& target : currentlyTracked){
				this->ConDensAte(in, out, target);
			}
			this->removeLostTargets();
			for(Target& target : currentlyTracked){
				if(target.features.size() < parameters.maxCorners){
					this->findNewFeatures(in, out, target);
				}
				this->filterFeatures(target);
			}
			this->removeLostTargets();

			for(Target& target : currentlyTracked){
				if(target.features.size() < parameters.maxCorners){
					this->findNewFeatures(in, out, target);
				}
			}
		}

		if(currentlyTracked.size() < parameters.generatingRange){
			this->pollNewTargets(in,out);
		}

		for(const Target& target : currentlyTracked){
			drawTarget(out, target);
		}
		//swap buffers
		cv::swap(grey_prev, grey);
		return;
	}

private:


	void removeLostTargets(){
		for(size_t i = 0; i < currentlyTracked.size();){
			if(currentlyTracked.at(i).features.empty()){
				currentlyTracked.erase(currentlyTracked.begin()+i);
			}
			else{
				++i;
			}
		}
	}

	Gaussian computePositionGaussian(Target& target){
		Gaussian gaussian;
		size_t k = 0;
		for(Feature& feature : target.features){
			gaussian.mean[0] += feature.point.x;
			gaussian.mean[1] += feature.point.y;
			++k;
		}
		gaussian.mean[0] /= k;
		gaussian.mean[1] /= k;
		int delta[2];
		for(Feature& feature : target.features){
			delta[0] = feature.point.x - gaussian.mean[0];
			delta[1] = feature.point.y - gaussian.mean[1];
			gaussian.deviation[0] += delta[0]*delta[0];
			gaussian.deviation[1] += delta[1]*delta[1];
		}
		gaussian.deviation[0] /= k;
		gaussian.deviation[1] /= k;

		gaussian.deviation[0] = sqrt(gaussian.deviation[0]);
		gaussian.deviation[1] = sqrt(gaussian.deviation[1]);

		return gaussian;
	}

	Gaussian computeShiftGaussian(Target& target){
		Gaussian gaussian;
		size_t k = 0;
		for(Feature& feature : target.features){
			gaussian.mean[0] += feature.point.x - feature.initial.x;
			gaussian.mean[1] += feature.point.y - feature.initial.y;
			++k;
		}
		gaussian.mean[0] /= k;
		gaussian.mean[1] /= k;
		int delta[2];
		for(Feature& feature : target.features){
			delta[0] = feature.point.x - feature.initial.x - gaussian.mean[0];
			delta[1] = feature.point.y - feature.initial.y - gaussian.mean[1];
			gaussian.deviation[0] += delta[0]*delta[0];
			gaussian.deviation[1] += delta[1]*delta[1];
		}
		gaussian.deviation[0] /= k;
		gaussian.deviation[1] /= k;

		gaussian.deviation[0] = sqrt(gaussian.deviation[0]);
		gaussian.deviation[1] = sqrt(gaussian.deviation[1]);

		return gaussian;
	}

	void findNewFeatures(const cv::Mat& in, cv::Mat& out, Target& target){

		Gaussian gaussian = computePositionGaussian(target);

		Rect roi;
		roi.x = std::max(gaussian.mean[0] - gaussian.deviation[0]*2, 0);
		roi.y = std::max(gaussian.mean[1] - gaussian.deviation[1]*2, 0);
		roi.width = gaussian.deviation[0]*4 + 1;
		roi.height = gaussian.deviation[1]*4 + 1;

		if(roi.x + roi.width > in.cols){
			roi.width = in.cols - roi.x;
		}

		if(roi.y + roi.height > in.rows){
			roi.height = in.rows - roi.y;
		}

		std::vector<cv::Point2f> corners;
		cv::goodFeaturesToTrack(grey(roi), corners,
								parameters.maxCorners - target.features.size(), parameters.qualityLevel,
								parameters.minDistance, maskArray,
								parameters.blockSize, parameters.useHarrisDetector, parameters.k);
		if(!corners.empty()){
			Deltas deltas;
			deltas.x = 0;
			deltas.y = 0;
			for(Point corner : corners){
				corner.x += roi.x;
				corner.y += roi.y;
				this->initDensity(in);
				density.x[corner.x] = parameters.MAX_DIST;
				density.y[corner.y] = parameters.MAX_DIST;
				spread(deltas);
				Feature feature;
				feature.point = feature.initial = corner;
				feature.density = density;
				target.features.push_back(feature);
				cv::circle(out, feature.point, 3, COLOR_NEW_FEATURE,-1);
			}
		}
	}

	Rects getMovingObjects() {

		cv::findContours(accumulator.getForeground(),
						 contours,
						 hierarchy,
						 CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );


		Rects rects;

		for(unsigned int i = 0; i < contours.size();){
			Rect rect = cv::boundingRect(contours.at(i));
			if(rect.width > parameters.TARGET_MAX_WIDTH
					|| rect.width < parameters.TARGET_MIN_WIDTH
					|| rect.height > parameters.TARGET_MAX_HEIGHT
					|| rect.height < parameters.TARGET_MIN_HEIGHT){
				contours.erase(contours.begin()+i);
				continue;
			}
			rects.push_back(rect);
			++i;
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
		if (rangeSize <= parameters.pollingRange){
			for(int x = distributions.x.min(); x < distributions.x.max(); ++x){
				for(int y = distributions.y.min(); y < distributions.y.max(); ++y){
					result.push_back(Point(x,y));
				}
			}
			return result;
		}
		for(size_t i = 0; i < parameters.pollingRange; ++i){
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

		for(size_t i = 0; i < target.features.size();){
			Feature& feature = target.features.at(i);
//			this->initDensity(in);
//			this->initDistances(in);
			DiscreteDistributions distributions = {
				DiscreteDistribution(feature.density.x.begin(), feature.density.x.end()),
				DiscreteDistribution(feature.density.y.begin(), feature.density.y.end())
			};

			Points points = generatePoints(distributions);
			if(!points.empty()){
	//			std::map<Score, cv::Point> scores;
				Points::iterator point = points.begin();
				cv::Point bestCandidate = *point;
				Distance minDist = this->matcher.computeDistance(grey_prev, grey, feature.point, bestCandidate);
	//			this->initRefCounter(in);

				for(++point; point != points.end(); ++point){

					Distance distance = this->matcher.computeDistance(grey_prev, grey, feature.point, *point);
	//				++refCounter.x[point.x];
	//				++refCounter.y[point.y];
	//				distances.x[point.x] += distance;
	//				distances.y[point.y] += distance;

	//				scores.insert(std::pair<Score, cv::Point>(distance, point));
					if(distance < minDist){
						bestCandidate = *point;
						minDist = distance;
						if(minDist == 0){
							cv::circle(out, *point, 3, cv::Scalar(0x3b, 0x59, 0x98),-1);
							break;
						}
					}

					cv::circle(out, *point, 3, cv::Scalar(0,0,255),-1);
				}

	//			for(size_t i = 0; i < refCounter.x.size(); ++i){
	//				if(refCounter.x[i] > 1){
	//					distances.x[i] /= refCounter.x[i];
	//				}
	//			}

	//			for(size_t i = 0; i < refCounter.y.size(); ++i){
	//				if(refCounter.y[i] > 1){
	//					distances.y[i] /= refCounter.y[i];
	//				}
	//			}

	//			for(size_t i = 0; i < distances.x.size(); ++i){
	//				if(refCounter.x[i] > 0){
	//					density.x[i] = (distances.x[i] >= MAX_DIST)? 0 : MAX_DIST - distances.x[i];
	//				}
	//			}

	//			for(size_t i = 0; i < distances.y.size(); ++i){
	//				if(refCounter.y[i] > 0){
	//					density.y[i] = (distances.y[i] >= MAX_DIST)? 0 : MAX_DIST - distances.y[i];
	//				}
	//			}

				if (minDist > parameters.MAX_DIST){
					target.features.erase(target.features.begin()+i);
					continue;
				}

				this->initDensity(in);
				feature.point = bestCandidate;
				density.x[feature.point.x] = density.y[feature.point.y] = parameters.MAX_DIST;

				Deltas deltas = this->shift(feature.density);
				this->spread(deltas);

				feature.density = density;
				++i;
			}
			else{
				target.features.erase(target.features.begin()+i);
			}
		}
	}

	void filterFeatures(Target& target){
		Gaussian gaussian = computeShiftGaussian(target);

		int delta[2] = {0,0};

		for(size_t i = 0; i < target.features.size();){
			delta[0] = abs((target.features.at(i).point.x - target.features.at(i).initial.x) - gaussian.mean[0]);
			delta[1] = abs((target.features.at(i).point.y - target.features.at(i).initial.y) - gaussian.mean[1]);
			if(delta[0] > gaussian.deviation[0]*2 || delta[1] > gaussian.deviation[1]*2){
				target.features.erase(target.features.begin()+i);
			}
			else{
				++i;
			}
		}
	}

	void pollNewTargets(const cv::Mat &in, cv::Mat &out){


		Rects rects = getMovingObjects(); //stored in contour

		cv::Scalar color = cv::Scalar(0,0x80,0);

		Deltas deltas;
		deltas.x = 0;
		deltas.y = 0;
		for(size_t i = 0; i < rects.size(); ++i){
			const Rect& rect = rects.at(i);
			cv::drawContours(out, contours, i, COLOR_WHITE);
			for(cv::Point& point : contours.at(i)){
				point.x -= rect.x;
				point.y -= rect.y;
			}
			std::vector<cv::Point2f> corners;

			cv::Mat mask = cv::Mat(rect.height, rect.width, CV_8UC1);
			mask.setTo(COLOR_BLACK);
			cv::drawContours(mask, contours, i, COLOR_WHITE);

			cv::goodFeaturesToTrack(grey(rect), corners,
									parameters.maxCorners, parameters.qualityLevel,
									parameters.minDistance, mask,
									parameters.blockSize, parameters.useHarrisDetector, parameters.k);
			if(!corners.empty()){
				Target target;
				target.features.reserve(corners.size());
				for(Point corner : corners){
					corner.x += rect.x;
					corner.y += rect.y;
					this->initDensity(in);
					density.x[corner.x] = parameters.MAX_DIST;
					density.y[corner.y] = parameters.MAX_DIST;
					spread(deltas);
					Feature feature;
					feature.point /*= feature.initial*/ = corner;
					feature.density = density;
					target.features.push_back(feature);
					cv::circle(out, feature.point, 3, color,-1);
				}


				currentlyTracked.push_back(target);
				std::cout << "pushed new target" << std::endl;
				cv::rectangle(out, rect, color);
			}
		}
	}

	Deltas shift(const Densities& prevDensity){
		Deltas deltas;
		deltas.x = shift(density.x, prevDensity.x);
		deltas.y = shift(density.y, prevDensity.y);
		return deltas;
	}

	void spread(Deltas deltas){
		spread(density.x, deltas.x, parameters.spreadRange);
		spread(density.y, deltas.y, parameters.spreadRange);
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
			density = temp;
		}

		return delta;
	}

	void spread(Density& density, Delta delta, int spreadRange){
		Density temp;
		size_t blurRange = abs(delta) + spreadRange;
		int size = density.size();
		for(size_t d = 0; d < blurRange; ++d){
			temp = density;
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
		}
	}



};
#endif // ALGORITHM_CUSTOMCONDENSATIONTEMPLATEV2_H
