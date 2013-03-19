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
		Delta x, y, w, h;
	} Deltas;

	typedef std::vector<double> RefCounter;

private:

	Generator generator = Generator(Clock::now().time_since_epoch().count());
	Targets currentlyTracked;
	size_t pollingRange = 50;
	size_t generatingRange = 1;
	int spreadRange = 20;
	int minScore = 50;
	int TARGET_MIN_WIDTH = 1; //should be at least 1 px and correspond to rect generator (eg tagging) results for no crash
	int TARGET_MIN_HEIGHT = 1;
	size_t MAX_DIST = 1000000;
	Densities density;
	Distances2D distances;
	RefCounter refCounter;
	Sub_Tagging tagging;


public:

	CustomCondensationTemplateV2(){}

	void process(const cv::Mat &in, cv::Mat &out){
		out = in.clone();

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

	void drawTarget(cv::Mat& out, const Target& target){
		if(!target.features.empty()){
			double mean[2] = 0;
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

	Points generatePoints(const cv::Mat &in, DiscreteDistributions &distributions){

		Rects result;
		for(size_t i = 0; i < pollingRange; ++i){
			result.push_back(Point(distributions.x(generator),distributions.y(generator)));
		}
		return result;
	}

	void initDensity(const cv::Mat& in){
		density.x.clear();
		density.y.clear();
		density.x.resize(in.cols, 0);
		density.y.resize(in.rows, 0);
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
		this->initDensity(in);

		for(Feature& feature : target.features){
			DiscreteDistributions distributions = {
				DiscreteDistribution(feature.density.x.begin(), feature.density.x.end()),
				DiscreteDistribution(feature.density.y.begin(), feature.density.y.end())
			};

			Points points = generatePoints(in, distributions);

			std::map<Score, cv::Point> scores;
			cv::Scalar color = cv::Scalar(0,0,0x80);
			this->initRefCounter(in);
			size_t k;

			for(const Point& point : points){

				Distance distance = this->matcher.computeDistance(in, feature.point, point);
				k = refCounter[point.x]++;
				distances.x[point.x] = distances.x[point.x]*(k/k+1) + distance/(k+1);
				k = refCounter[point.y]++;
				distances.y[point.y] = distances.y[point.y]*(k/k+1) + distance/(k+1); //floating point division

				scores.insert(std::pair<Score, cv::Point>(score, point));

				cv::circle(out, point, 3, cv::Scalar(0,0,255),-1);
			}

			for(Distance distance : distances.x){
				density.x = (distance >= MAX_DIST)? 0 : MAX_DIST - distance;
			}

			for(Distance distance : distances.y){
				density.y = (distance >= MAX_DIST)? 0 : MAX_DIST - distance;
			}


			Deltas deltas = this->shift(feature.density);
			this->spread(deltas);

			feature.density = density;
			feature.point = scores.begin()->second;
		}
	}

	void pollNewTargets(const cv::Mat &in, cv::Mat &out){

		this->initDensity(in);

		Rects rects = tagging.process(in);//generateRects(in);

		std::map<int, Rect> scores;
		cv::Scalar color = cv::Scalar(0,0x80,0);
		for(Rect rect : rects){

			if(abs(rect.width - model.cols) > 50 || abs(rect.height - model.rows) > 50) continue;

			std::cout << "Matching rect : (" << rect.x << ',' << rect.y << ',' << rect.width << ',' << rect.height << ')' << std::endl;

			int score = this->matcher.computeScore(in(rect), model);
			density.x[rect.x] += score;
			density.y[rect.y] += score;
			density.w[rect.width] += score;
			density.h[rect.height] += score;

			scores.insert(std::pair<int,Rect>(score, rect));

			cv::rectangle(out, rect, color);

			cv::Point point;
			point.x = rect.x + rect.width + 10;
			point.y = rect.y + rect.height + 10;

			std::ostringstream text;
			text << score;
			cv::putText(out, text.str(), point, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, color, 1, CV_AA);

			std::cout << score << std::endl;
		}

		std::map<int, Rect>::const_reverse_iterator it = scores.rbegin();

		for(size_t i = 0; i < generatingRange && i < scores.size()  ; ++i, ++it){
			if(it->first < minScore){
				break;
			}
			currentlyTracked.push_back(Target(in, it->second, density));
			std::cout << "pushed new target" << std::endl;
		}
	}

	Deltas shift(const Densities& prevDensity){
		Deltas deltas;
		deltas.x = shift(density.x, prevDensity.x);
		deltas.y = shift(density.y, prevDensity.y);
		deltas.w = shift(density.w, prevDensity.w);
		deltas.h = shift(density.h, prevDensity.h);
		return deltas;
	}

	void spread(Deltas deltas){
		spread(density.x, deltas.x, spreadRange);
		spread(density.y, deltas.y, spreadRange);
		spread(density.w, deltas.w, spreadRange);
		spread(density.h, deltas.h, spreadRange);
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
		int blurRange = abs(delta) + spreadRange;
		int size = density.size();
		for(int i = 0; i < size; ++i){
			int val = 0;
			int k = 0;
			int begin = std::max(0,i-blurRange/2);
			int end = std::min(size,i+blurRange/2);
			for(int j = begin; j < end; ++j, ++k){
				val += temp[j];
			}
			if(k > 0) density[i] = val/k;
		}
	}



};
#endif // ALGORITHM_CUSTOMCONDENSATIONTEMPLATEV2_H
