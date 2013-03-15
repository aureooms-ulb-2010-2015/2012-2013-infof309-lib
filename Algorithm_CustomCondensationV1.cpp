#include "Algorithm_CustomCondensationV1.h"
#include <random>
#include <iostream>
#include <map>
#include <math.h>
#include <algorithm>



CustomCondensationV1::CustomCondensationV1(){
	if(!model.data) std::cout << "can't open file" << std::endl;
}


CustomCondensationV1::Points CustomCondensationV1::generatePoints(const cv::Mat &in){
	UniformDistributions distributions = {
		UniformDistribution(0, in.cols),
		UniformDistribution(0, in.rows)
	};

	Points result;
	for(size_t i = 0; i < pollingRange; ++i){
		result.push_back(Point(distributions.w(generator),distributions.h(generator)));
	}
	return result;
}

CustomCondensationV1::Rects CustomCondensationV1::generateRects(const cv::Mat &in){

	//size_t max = model.cols > model.rows ? model.cols : model.rows;

	UniformDistributions posDistributions = {
		UniformDistribution(0, in.cols-model.cols),
		UniformDistribution(0, in.rows-model.rows)
	};

	/*UniformDistributions sizeDistributions = {
		UniformDistribution(1, model.cols),
		UniformDistribution(1, model.rows)
	};*/

	Rects result;
	for(size_t i = 0; i < pollingRange; ++i){
		int x = posDistributions.w(generator);
		int y = posDistributions.h(generator);
		int w = model.cols;//sizeDistributions.w(generator);
		if (w > (in.cols - x)){
			w = in.cols - x;
		}
		int h = model.rows;//sizeDistributions.h(generator);
		if (h > (in.rows - y)){
			h = in.rows - y;
		}

		result.push_back(Rect(x,y,w,h));//Rect(0,0,in.cols,in.rows)
	}
	return result;
}

CustomCondensationV1::Rects CustomCondensationV1::generateRects(const cv::Mat &in, DiscreteDistributions &distributions){

	Rects result;
	for(size_t i = 0; i < pollingRange; ++i){
		int x = distributions.x(generator);
		int y = distributions.y(generator);
		int w = distributions.w(generator);
		if (w > (in.cols - x)){
			w = in.cols - x;
		}
		int h = distributions.h(generator);
		if (h > (in.rows - y)){
			h = in.rows - y;
		}

		result.push_back(Rect(x,y,w,h));
	}
	return result;
}

void CustomCondensationV1::initDensity(const cv::Mat& in){

	density.x.clear();
	density.y.clear();
	density.w.clear();
	density.h.clear();

	density.x.resize(in.cols, 0);
	density.y.resize(in.rows, 0);
	density.w.resize(in.cols, 0);
	density.h.resize(in.rows, 0);
}

void CustomCondensationV1::process(const cv::Mat &in, cv::Mat &out){
	out = in.clone();

	for(Target& target : currentlyTracked){
		this->ConDensAte(in, out, target);
	}

	if(currentlyTracked.size() < generatingRange){
		this->pollNewTargets(in,out);
	}

	//for(Target& target : currentlyTracked){
	//	cv::rectangle(out, target.rect, cv::Scalar(0xb4, 0x84, 0x6d));
	//}


	return;
}

//cv::rectangle(out,rect,cv::Scalar(0xb4, 0x84, 0x6d));

void CustomCondensationV1::ConDensAte(const cv::Mat &in, cv::Mat &out, Target& target){
	this->initDensity(in);

	DiscreteDistributions distributions = {
		DiscreteDistribution(target.density.x.begin(), target.density.x.end()),
		DiscreteDistribution(target.density.y.begin(), target.density.y.end()),
		DiscreteDistribution(target.density.w.begin(), target.density.w.end()),
		DiscreteDistribution(target.density.h.begin(), target.density.h.end())
	};

	Rects rects = generateRects(in, distributions);

	std::map<int, Rect> scores;
	cv::Scalar color = cv::Scalar(0,0,0x80);

	for(Rect rect : rects){

		int score = matcher.computeScore(in(rect), target.picture);
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


	Deltas deltas = this->shift(target.density);
	this->spread(deltas);

	target.density = density;

	target.rect = scores.rbegin()->second;

	target.picture = in(target.rect).clone();

	// define destination for resized image
	cv::Rect roi( cv::Point( 0, 0 ), target.picture.size());
	cv::Mat destination = out(roi);

	// copy resized image to destination in out
	target.picture.copyTo(destination);

}

void CustomCondensationV1::pollNewTargets(const cv::Mat &in, cv::Mat &out){

	this->initDensity(in);

	Rects rects = tagging.process(in);//generateRects(in);

	std::map<int, Rect> scores;
	cv::Scalar color = cv::Scalar(0,0x80,0);
	for(Rect rect : rects){

		if(abs(rect.width - model.cols) > 50 || abs(rect.height - model.rows) > 50) continue;

		std::cout << "Matching rect : (" << rect.x << ',' << rect.y << ',' << rect.width << ',' << rect.height << ')' << std::endl;

		int score = matcher.computeScore(in(rect), model);
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
/*
void CustomCondensationV1::computeDensity(const cv::Mat &in, Rects rects){
	for(Rect rect : rects){

		int score = matcher.computeScore(in(rect), model);
		density.x[rect.x] += score;
		density.y[rect.y] += score;
		density.w[rect.width] += score;
		density.h[rect.height] += score;

		std::cout << score << std::endl;
	}
}

void CustomCondensationV1::computeDensity(const cv::Mat &in, const CustomCondensationV1::Target &target){

	DiscreteDistributions distributions = {
		DiscreteDistribution(target.score.x.begin(), target.score.x.end()),
		DiscreteDistribution(target.score.y.begin(), target.score.y.end()),
		DiscreteDistribution(target.score.w.begin(), target.score.w.end()),
		DiscreteDistribution(target.score.h.begin(), target.score.h.end())
	};

	Rects rects = generateRects(in, distributions);

	for(Rect rect : rects){

		int score = matcher.computeScore(in(rect), target.picture);
		density.x[rect.x] += score;
		density.y[rect.y] += score;
		density.w[rect.width] += score;
		density.h[rect.height] += score;
	}
}
*/

CustomCondensationV1::Deltas CustomCondensationV1::shift(const Densities& prevDensity){
	Deltas deltas;
	deltas.x = shift(density.x, prevDensity.x);
	deltas.y = shift(density.y, prevDensity.y);
	deltas.w = shift(density.w, prevDensity.w);
	deltas.h = shift(density.h, prevDensity.h);
	return deltas;
}

void CustomCondensationV1::spread(Deltas deltas){
	spread(density.x, deltas.x, spreadRange);
	spread(density.y, deltas.y, spreadRange);
	spread(density.w, deltas.w, spreadRange);
	spread(density.h, deltas.h, spreadRange);
}

CustomCondensationV1::Delta CustomCondensationV1::shift(Density& density, const Density& prevDensity){

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

void CustomCondensationV1::spread(Density& density, Delta delta, int spreadRange){
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

