#ifndef DISCRETEDISTRIBUTION_H
#define DISCRETEDISTRIBUTION_H

#include <unordered_map>
#include <random>


template<typename V, typename P, typename U = double, typename D = double>class DiscreteDistribution{

private:

	std::unordered_map<V, P> distribution;
	std::unordered_map<V, P>::const_iterator distribution_it;
	std::unordered_map<V, D> density;
	std::unordered_map<V, D>::const_iterator density_it;

public:
	DiscreteDistribution(){

	}

	V operator() (U u){
		for(density_it = density.begin(); density_it+1 != density.end(); ++density_it){
			if(u <= (*density_it).second){
				return (*density_it).first;
			}
		}
		return (*density_it).first;
	}

	void computeDensity(){
		D total = 0;
		for(distribution_it = distribution.begin(); distribution_it != distribution.end()-1; ++distribution_it){
			total += (*distribution_it).second;
		}

		D dens_x = 0;
		for(distribution_it = distribution.begin(); distribution_it != distribution.end()-1; ++distribution_it){
			dens_x += (*distribution_it).second;
			density.insert(std::pair<V,D>((*distribution_it).first, dens_x/total));
		}
	}



};

#endif // DISCRETEDISTRIBUTION_H
