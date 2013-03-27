#ifndef ALGORITHM_CUSTOMCONDENSATIONTEMPLATEV3_H
#define ALGORITHM_CUSTOMCONDENSATIONTEMPLATEV3_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>



#include <chrono>
#include <vector>
#include <random>
#include <iostream>
#include <map>
#include <math.h>
#include <algorithm>

#include "CondensationParameters.h"
#include "Algorithm_AccumulateBackground.h"
#include "FrameProcessor.h"
#include "Target_CondensationTargetV3.h"

class CustomCondensationTemplateV3: public FrameProcessor{

public:

	typedef std::chrono::high_resolution_clock Clock;
	typedef cv::Point Point;
	typedef std::vector<Point> Points;
	typedef cv::Rect Rect;
	typedef std::vector<Rect> Rects;
	typedef CondensationTargetV3 Target;
	typedef DistanceMatcher::Distance Distance;
	typedef std::vector<Distance> Distances;
	typedef struct{
		Distances x;
		Distances y;
	} Distances2D;
	typedef Target::Density Density;
	typedef Target::Densities Densities;
	typedef Target::Feature Feature;
	typedef Target::Features Features;
	typedef std::vector<Target> Targets;
	typedef std::normal_distribution<float> NormalDistribution;
	typedef struct{
		NormalDistribution x;
		NormalDistribution y;
	} NormalDistributions;

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


	CondensationParameters parameters;

private:
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

	CustomCondensationTemplateV3(){}

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
//				if(target.features.size() < parameters.maxCorners){
//					this->findNewFeatures(in, out, target);
//				}
				this->filterFeatures(target);
			}
			this->removeLostTargets();

//			for(Target& target : currentlyTracked){
//				if(target.features.size() < parameters.maxCorners){
//					this->findNewFeatures(in, out, target);
//				}
//			}
		}

		if(currentlyTracked.size() < parameters.generatingRange){
			this->pollNewTargets(out);
		}

		for(const Target& target : currentlyTracked){
			drawTarget(out, target);
		}

		cv::swap(grey_prev, grey);
		return;
	}

private:


	void removeLostTargets(){
		for(size_t i = 0; i < currentlyTracked.size();){
			if(currentlyTracked.at(i).features.size() < parameters.MIN_FEATURES){
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
				density.x.mean = corner.x;
				density.y.mean = corner.y;
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

	Points generatePoints(const cv::Mat& in, NormalDistributions &distributions){

		Points result;
		/*size_t rangeSize = (distributions.x.max() - distributions.x.min()) * (distributions.y.max() - distributions.y.min());
		if (rangeSize <= parameters.pollingRange){
			for(int x = distributions.x.min(); x < distributions.x.max(); ++x){
				for(int y = distributions.y.min(); y < distributions.y.max(); ++y){
					result.push_back(Point(x,y));
				}
			}
			return result;
		}*/
		int x, y;
		for(size_t i = 0; i < parameters.pollingRange; ++i){
			x = distributions.x(generator);
			y = distributions.y(generator);
			x = std::max(0, std::min(in.cols - 1, x));
			y = std::max(0, std::min(in.rows - 1, y));
			result.push_back(Point(x,y));
		}
		return result;
	}

	void ConDensAte(const cv::Mat& in, cv::Mat &out, Target& target){

		for(size_t i = 0; i < target.features.size();){
			Feature& feature = target.features.at(i);
			NormalDistributions distributions = {
				NormalDistribution(feature.density.x.mean, feature.density.x.deviation),
				NormalDistribution(feature.density.y.mean, feature.density.y.deviation)
			};
			std::map<Distance, cv::Point> bestCandidates;
			for(int k = 0; k < parameters.resamplingPasses; ++k){
				Points points = generatePoints(in, distributions);

				if(points.empty()) break;

				Points::iterator point = points.begin();
				for(int b = 0; b < parameters.resamplingRange && point != points.end(); ++b){
					Distance distance = parameters.matcher->computeDistance(grey_prev, grey, feature.point, *point);
					bestCandidates.insert(std::pair<Distance, cv::Point>(distance, *point));
					if(distance == 0){
						cv::circle(out, *point, 3, cv::Scalar(0x3b, 0x59, 0x98),-1);
						break;
					}
					++point;
				}
				if(bestCandidates.begin()->first != 0){
					for(; point != points.end(); ++point){

						Distance distance = parameters.matcher->computeDistance(grey_prev, grey, feature.point, *point);

						if(distance < bestCandidates.rbegin()->first){
							bestCandidates.erase(--bestCandidates.end());
							bestCandidates.insert(std::pair<Distance, cv::Point>(distance, *point));
							if(distance == 0){
								cv::circle(out, *point, 3, cv::Scalar(0x3b, 0x59, 0x98),-1);
								break;
							}
						}

						cv::circle(out, *point, 3, cv::Scalar(0,0,255),-1);
					}
				}
				else{
					break;
				}

				if(bestCandidates.begin()->first == 0) break;

				if(k+1 < parameters.resamplingPasses){

					double mean[2] = {0,0};
					double deviation[2] = {0,0};
					int ech = 0;
					for(std::map<Distance, cv::Point>::iterator b = bestCandidates.begin(); b != bestCandidates.end(); ++b){
						mean[0] += b->second.x;
						mean[1] += b->second.y;
						++ech;
					}

					mean[0] /= ech;
					mean[1] /= ech;


					for(std::map<Distance, cv::Point>::iterator b = bestCandidates.begin(); b != bestCandidates.end(); ++b){
						double temp;
						temp = b->second.x - mean[0];
						deviation[0] += temp*temp;
						temp = b->second.y - mean[1];
						deviation[1] += temp*temp;
					}

					deviation[0] /= ech;
					deviation[1] /= ech;
					deviation[0] = sqrt(deviation[0]);
					deviation[1] = sqrt(deviation[1]);

					distributions = {
						NormalDistribution(mean[0], deviation[0]),
						NormalDistribution(mean[1], deviation[1])
					};
				}
			}

			if(!bestCandidates.empty()){
				if (bestCandidates.begin()->first > parameters.MAX_DIST){
					target.features.erase(target.features.begin()+i);
					continue;
				}

				feature.point = bestCandidates.begin()->second;
				density.x.mean = feature.point.x;
				density.y.mean = feature.point.y;

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
//		Gaussian gaussian = computeShiftGaussian(target);

//		int delta[2] = {0,0};

//		for(size_t i = 0; i < target.features.size();){
//			delta[0] = abs((target.features.at(i).point.x - target.features.at(i).initial.x) - gaussian.mean[0]);
//			delta[1] = abs((target.features.at(i).point.y - target.features.at(i).initial.y) - gaussian.mean[1]);
//			if(delta[0] > gaussian.deviation[0]*2 || delta[1] > gaussian.deviation[1]*2){
//				target.features.erase(target.features.begin()+i);
//			}
//			else{
//				++i;
//			}
//		}
		for(size_t i = 0; i < target.features.size();){
			int a, b;
			a = abs(target.features.at(i).point.x - target.features.at(i).previous.x);
			b = abs(target.features.at(i).point.y - target.features.at(i).previous.y);
			if(a > 0 || b > 0){
				target.features.at(i).trust += (a+b)*parameters.TRUST_BONUS_FACTOR;
			}
			else{
				target.features.at(i).trust -= parameters.TRUST_MALUS;
			}
			if(target.features.at(i).trust <= parameters.TRUST_DIE){
				target.features.erase(target.features.begin()+i);
			}
			else{
				target.features.at(i).previous = target.features.at(i).point;
				++i;
			}
		}

	}

	void pollNewTargets(cv::Mat &out){


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
			const cv::Point* poly_contours[1];
			poly_contours[0] = contours[i].data();
			int poly_contours_n[1];
			poly_contours_n[0] = contours[i].size();

			//cv::drawContours(mask, contours, i, COLOR_WHITE);
			cv::fillPoly(mask, poly_contours, poly_contours_n, 1, COLOR_WHITE);

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
					density.x.mean = corner.x;
					density.y.mean = corner.y;
					spread(deltas);
					Feature feature;
					feature.point = feature.initial = feature.previous =  corner;
					feature.density = density;
					feature.trust = parameters.TRUST_START;
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
		Delta delta = density.mean - prevDensity.mean;
		density.mean += delta;
		return delta;
	}

	void spread(Density& density, Delta delta, int spreadRange){
		density.deviation = abs(delta) + spreadRange;
	}



};
#endif // ALGORITHM_CUSTOMCONDENSATIONTEMPLATEV3_H
