#include "Algorithm_FeatureProjection.h"
#include <cmath>
#include <iostream>

FeatureProjection::FeatureProjection():tagging(10, 0.01, cv::Size(10,10)){}

void FeatureProjection::process(const cv::Mat &in, cv::Mat &out) {
	int nbFishs = 0;

	out = in.clone();

	std::vector<Target> newTargets;

	for(cv::Rect rect : tagging.process(in)){
		newTargets.push_back(Target(rect));
		cv::rectangle(out, rect, cv::Scalar(0,0,0));
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
	for(const Match& match : matches){
		size_t i = match.first;
		size_t j = match.second;
		newTargets.at(j).age = this->_previousTargets.at(i).age + AGE_BONUS;
		newTargets.at(j).id  = this->_previousTargets.at(i).id;

		//UPDATE PROJECTION
		for(size_t l = 1; l < D; ++l){
			newTargets.at(j).x[l] = newTargets.at(j).x[l-1] - this->_previousTargets.at(i).x[l-1];
			newTargets.at(j).y[l] = newTargets.at(j).y[l-1] - this->_previousTargets.at(i).y[l-1];
		}

		if(newTargets.at(j).age > AGE_TRUST){
			drawTarget(out, newTargets.at(j), cv::Scalar(0, 122, 0));
			++nbFishs;
		}
		else if(newTargets.at(j).age > AGE_MID){
			drawTarget(out, newTargets.at(j), cv::Scalar(0, 105, 250));
		}
	}

	//REMOVE MATCHED OLD TARGETS
	std::vector<bool> isMatched_previousTargets(this->_previousTargets.size(),false);

	for(const Match& match : matches){
		isMatched_previousTargets[match.first] = true;
	}

	size_t j = 0;
	for(size_t k = 0; k < isMatched_previousTargets.size(); ++k){
		size_t i = k-j;
		if(isMatched_previousTargets[k]){
			std::cout << "match " << this->_previousTargets.at(i).id << ", " << this->_previousTargets.at(i).age << std::endl;
			this->_previousTargets.erase(this->_previousTargets.begin()+i);
			++j;
		}
	}



	//DECREMENT AGE ON MISSING TARGETS
	for(size_t i = 0; i < this->_previousTargets.size(); ++i){
		cv::rectangle(out, this->_previousTargets.at(i).rect, cv::Scalar(90,0,90));
		this->_previousTargets.at(i).age -= AGE_PENALTY;
		if(this->_previousTargets.at(i).age <= AGE_DIE){
			std::cout << "delete " << this->_previousTargets.at(i).id << ", " << this->_previousTargets.at(i).age << std::endl;
			this->_previousTargets.erase(this->_previousTargets.begin()+i);
			--i;
		}
		else{
			std::cout << "keep " << this->_previousTargets.at(i).id << ", " << this->_previousTargets.at(i).age << std::endl;
		}
	}

	//PROJECTION FOR MISSING TARGETS

	for(Target& target : this->_previousTargets){
		size_t begin = 1;
		Feature delta_x = target.x[begin];
		Feature delta_y = target.y[begin];
		for(size_t l = begin; l > 0; --l){
			Feature temp_x = target.x[l-1];
			Feature temp_y = target.y[l-1];
			target.x[l-1] += delta_x;
			target.y[l-1] += delta_y;
			delta_x = temp_x + delta_x/2;
			delta_y = temp_y + delta_y/2;
		}

		cv::Rect previous = target.rect;
		Feature x = target.x[0] - previous.width/2;
		Feature y = target.y[0] - previous.height/2;
		target.rect = cv::Rect(x,y,previous.width,previous.height);

		drawTarget(out, target, cv::Scalar(122, 0, 0));
	}

	//FUTURE FOR NEW TARGETS
	for(const Target& target : newTargets){
		Feature x = target.x[D-1];
		Feature y = target.y[D-1];
		for(size_t l = D - 1; l > 0; --l){
			x = target.x[l-1] + x/2;
			y = target.y[l-1] + y/2;
		}

		cv::Rect previous = target.rect;
		x -= previous.width/2;
		y -= previous.height/2;

		cv::rectangle(out, cv::Rect(x,y,previous.width,previous.height), cv::Scalar(0, 0, 122));
	}


	//GENERATE IDS FOR NEW TARGETS

	for(Target& target : newTargets){
		if(target.id != "") continue;
		std::ostringstream stream;
		stream << ++NEXT_ID;
		target.id = stream.str();
	}

	//ADD NEW TARGETS TO HISTORY
	this->_previousTargets.insert(this->_previousTargets.end(),newTargets.begin(),newTargets.end());
	//std::cout << this->_previousTargets.size() << std::endl;
	this->writeFishCount(out,nbFishs);
	return;
}

FeatureProjection::MatchingScores FeatureProjection::generateMatchingScores(const Targets& newTargets) const{

	MatchingScores scores;


	for(size_t i = 0; i < this->_previousTargets.size(); ++i){
		Target pt = this->_previousTargets.at(i);
		for(size_t j = 0; j < newTargets.size(); ++j){
			int ndelta_h = abs(newTargets.at(j).rect.height - pt.rect.height);
			int ndelta_w = abs(newTargets.at(j).rect.width - pt.rect.width);
			int ndelta_x = abs(newTargets.at(j).x[0] - pt.x[0]);
			int ndelta_y = abs(newTargets.at(j).y[0] - pt.y[0]);
			if(ndelta_x > MAXDIST || ndelta_y > MAXDIST || ndelta_h > MAXTRANSFORM || ndelta_w > MAXTRANSFORM){
				continue;
			}
			int score = -(ndelta_h + ndelta_w + ndelta_x + ndelta_y);
			Match couple(i,j);
			scores.insert(MatchingScore(score, couple));
		}
	}

	return scores;
}

FeatureProjection::Matches FeatureProjection::generateMatches(const MatchingScores& scores, size_t newTargetsSize) const{
	std::vector<bool> isFree_previousTargets(this->_previousTargets.size(),true);
	size_t amount_freePreviousTargets = this->_previousTargets.size();

	std::vector<bool> isFree_newTargets(newTargetsSize,true);
	size_t amount_freeNewTargets = newTargetsSize;

	MatchingScores::const_reverse_iterator it_scores = scores.rbegin();

	Matches result;
	while(amount_freePreviousTargets > 0 && amount_freeNewTargets > 0 && it_scores != scores.rend()){
		size_t i = (*it_scores).second.first;
		size_t j = (*it_scores).second.second;

		if(isFree_previousTargets[i] && isFree_newTargets[j]){
			result.push_back(Match(i,j));
			isFree_previousTargets[i] = false;
			isFree_newTargets[j] = false;
			--amount_freePreviousTargets;
			--amount_freeNewTargets;
		}
		++it_scores;
	}
	return result;
}

void FeatureProjection::writeFishCount(cv::Mat& out, int nbFishs){


	cv::Point point;
	point.x = 20;
	point.y = 20;


	std::ostringstream text;
	text << "Number of Fishs : " << nbFishs;

	cv::putText(out, text.str(), point, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(255,0,0), 1, CV_AA);
}

void FeatureProjection::drawTarget(cv::Mat &out, const Target &target, cv::Scalar color){
	cv::rectangle(out, target.rect, color, 1, 8, 0 );

	cv::Point point;
	point.x = target.rect.x + target.rect.width + 10;
	point.y = target.rect.y - 10;

	std::ostringstream text;
	text << target.age;
	cv::putText(out, text.str(), point, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, color, 1, CV_AA);


	point.x = target.rect.x + target.rect.width + 10;
	point.y = target.rect.y + target.rect.height + 10;

	cv::putText(out, target.id, point, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, color, 1, CV_AA);

}
