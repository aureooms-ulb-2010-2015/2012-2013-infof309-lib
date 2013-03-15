#include "Algorithm_OomsChallenge.h"
#include <cmath>
#include <iostream>

OomsChallenge::OomsChallenge():_threshold(10), _learningRate(0.01){}

void OomsChallenge::process(const cv::Mat &in, cv::Mat &out) {
	const int AGE_TRUST = 64;
	const int AGE_MID = 24;
	const int AGE_DIE = 12;
	const int AGE_PENALTY = 1;
	const int AGE_BONUS = 2;
	const int MAXSIZE = 40;
	const int MINSIZE = 5;
	int nbFishs = 0;
	cv::Size gaussian(10,10);

	out = in.clone();
	//return;
	// convert input frame to gray-level image and blur it
	cv::cvtColor(out, this->_gray, CV_BGR2GRAY);
	cv::blur( this->_gray, this->_gray, gaussian );

	// initialize background to 1st frame
	if (this->_background.empty()){
		this->_gray.convertTo(this->_background, CV_32F);// 32 bit floating point
	}

	// convert background to unsigned 8bit/pixel (values 0-255)
	this->_background.convertTo(this->_backImage, CV_8U);

	// compute difference between current image and background
	cv::absdiff(this->_backImage, this->_gray, this->_foreground);

	// contrast the image
	cv::threshold(this->_foreground, this->_foreground, 12, 255, cv::THRESH_BINARY_INV);

	//CV_RETR_CCOMP
	//foreground = 255 - foreground;
	int erosion_type = cv::MORPH_RECT;
	int erosion_size = 1;
	cv::Mat element = cv::getStructuringElement( erosion_type, cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ), cv::Point( erosion_size, erosion_size ) );
	cv::morphologyEx(this->_foreground,this->_foreground,cv::MORPH_OPEN,element);
	cv::erode(this->_foreground,this->_foreground,cv::Mat());
	cv::dilate(this->_foreground,this->_foreground,cv::Mat());


	std::vector<std::vector<cv::Point> > contour;
	cv::findContours(this->_foreground, contour, this->_hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	cv::Mat temp;
	// apply threshold to foreground image
	cv::threshold(this->_foreground, temp, this->_threshold, 255, cv::THRESH_BINARY_INV);
	// accumulate background
	cv::accumulateWeighted(this->_gray, this->_background, this->_learningRate, temp);

	std::vector<Target> newTargets;
	//Get & Draw the bounding rectangle around the moving object
	for(unsigned int i = 0; i < contour.size(); ++i){
		cv::Rect rect =  cv::boundingRect(contour.at(i));
		newTargets.push_back(Target(rect));
	}

	for(size_t i = 0; i < newTargets.size();){
		if (newTargets.at(i).rect.width > MAXSIZE ||
				newTargets.at(i).rect.width < MINSIZE ||
				newTargets.at(i).rect.height > MAXSIZE ||
				newTargets.at(i).rect.height < MINSIZE){
			newTargets.erase(newTargets.begin()+i);
			continue;
		}
		++i;
	}

	std::multimap<int, std::pair<size_t,size_t> > scores = this->generateMatchingScores(newTargets);
	std::vector<std::pair<size_t,size_t> > matches = this->generateMatches(scores, newTargets.size());


	//UPDATE AND DRAW MATCHED ITEMS
	for(size_t k = 0; k < matches.size(); ++k){
		size_t i = matches.at(k).first;
		size_t j = matches.at(k).second;
		newTargets.at(j).age = this->_previousTargets.at(i).age + AGE_BONUS;
		if(newTargets.at(j).age > AGE_TRUST){
			cv::rectangle(out, newTargets.at(j).rect, cv::Scalar(0, 122, 0), 1, 8, 0 );
			++nbFishs;
		}
		else if(newTargets.at(j).age > AGE_MID){
			cv::rectangle(out, newTargets.at(j).rect, cv::Scalar(0, 105, 250), 1, 8, 0 );
		}
	}

	//REMOVE MATCHED OLD TARGETS
	std::vector<bool> isMatched_previousTargets;
	for(size_t i = 0; i < this->_previousTargets.size(); ++i){
		isMatched_previousTargets.push_back(false);
	}

	for(size_t k = 0; k < matches.size(); ++k){
		size_t i = matches.at(k).first;
		isMatched_previousTargets[i] = true;
	}

	size_t j = 0;
	for(size_t k = 0; k < isMatched_previousTargets.size(); ++k){
		size_t i = k-j;
		if(isMatched_previousTargets[k]){
			this->_previousTargets.erase(this->_previousTargets.begin()+i);
			++j;
		}
	}

	//DECREMENT AGE ON OLD TARGETS
	for(size_t i = 0; i < this->_previousTargets.size(); ++i){
		this->_previousTargets.at(i).age -= AGE_PENALTY;
		if(this->_previousTargets.at(i).age > AGE_DIE){
			cv::rectangle(out, this->_previousTargets.at(i).rect, cv::Scalar(122, 0, 0), 1, 8, 0 );
		}
		else{
			this->_previousTargets.erase(this->_previousTargets.begin()+i);
			--i;
		}
	}

	//ADD NEW TARGETS TO HISTORY
	this->_previousTargets.insert(this->_previousTargets.end(),newTargets.begin(),newTargets.end());

	this->writeFishCount(out,nbFishs);
	return;
}

std::multimap<int, std::pair<size_t,size_t> > OomsChallenge::generateMatchingScores(const std::vector<Target>& newTargets) const{
	const int MAXDIST = 40;
	const int MAXTRANSFORM = 40;

	std::multimap<int, std::pair<size_t,size_t> > scores;


	for(size_t i = 0; i < this->_previousTargets.size(); ++i){
		Target pt = this->_previousTargets.at(i);
		for(size_t j = 0; j < newTargets.size(); ++j){
			int ndelta_h = abs(newTargets.at(j).rect.height - pt.rect.height);
			int ndelta_w = abs(newTargets.at(j).rect.width - pt.rect.width);
			int ndelta_x = abs(newTargets.at(j).rect.x - pt.rect.x);
			int ndelta_y = abs(newTargets.at(j).rect.y - pt.rect.y);
			if(ndelta_x > MAXDIST || ndelta_y > MAXDIST || ndelta_h > MAXTRANSFORM || ndelta_w > MAXTRANSFORM){
				continue;
			}
			int score = -(ndelta_h + ndelta_w + ndelta_x + ndelta_y);
			std::pair<size_t,size_t> couple(i,j);
			scores.insert(std::pair<int, std::pair<size_t,size_t> >(score, couple));
		}
	}

	return scores;
}

std::vector<std::pair<size_t,size_t> > OomsChallenge::generateMatches(const std::multimap<int, std::pair<size_t,size_t> >& scores, size_t newTargetsSize) const{
	std::vector<bool> isFree_previousTargets;
	for(size_t i = 0; i < this->_previousTargets.size(); ++i){
		isFree_previousTargets.push_back(true);
	}
	size_t amount_freePreviousTargets = this->_previousTargets.size();

	std::vector<bool> isFree_newTargets;
	for(size_t i = 0; i < newTargetsSize; ++i){
		isFree_newTargets.push_back(true);
	}
	size_t amount_freeNewTargets = newTargetsSize;

	std::multimap<int, std::pair<size_t,size_t> >::const_reverse_iterator it_scores = scores.rbegin();

	std::vector<std::pair<size_t,size_t> > result;
	while(amount_freePreviousTargets > 0 && amount_freeNewTargets > 0 && it_scores != scores.rend()){
		size_t i = (*it_scores).second.first;
		size_t j = (*it_scores).second.second;

		if(isFree_previousTargets[i] && isFree_newTargets[j]){
			result.push_back(std::pair<size_t,size_t>(i,j));
			isFree_previousTargets[i] = false;
			isFree_newTargets[j] = false;
			--amount_freePreviousTargets;
			--amount_freeNewTargets;
		}
		++it_scores;
	}
	return result;
}

void OomsChallenge::writeFishCount(cv::Mat& out, int nbFishs){


	cv::Point point;
	point.x=20;
	point.y=20;


	std::string fishs_ = static_cast<std::ostringstream*>( &(std::ostringstream() << nbFishs) )->str();

	cv::putText(out, "Number of Fishs : "+fishs_, point, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(255,0,0), 1, CV_AA);
}
