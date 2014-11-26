#include "ParticleFilter.h"
#include "relax_lib.h"

//calculate particle weights (mWeights) and find highest likelihood particle. 
//computes an optimal annealing factor and scales the likelihoods. 
//Also removes any particles reported as invalid by the model.
template<class T>
void ParticleFilter<T>::CalcWeights(std::vector<Vectorf > &particles)
{	
	bool valid;
	mBestParticle = 0;
	fpType total = 0, best = 0, minWeight = 1e30f, annealingFactor = 1;
	mWeights.resize(particles.size());
	uint i = 0;
	while(i < particles.size())											//compute likelihood weights for each particle
	{	mWeights[i] = mModel->LogLikelihood(particles[i], valid); //slow
		if(!valid)														//if not valid(model prior), remove the particle from the list
		{	particles[i] = particles[particles.size() - 1];
			particles.pop_back(); mWeights.pop_back();
		}
		else
			minWeight = std::min(mWeights[i++], minWeight);				//find minimum weight
	}
	if((int)particles.size() < mMinParticles) return;					//bail out if not enough valid particles
	mWeights -= minWeight;												//shift weights to zero for numerical stability
	if(mModel->StdDevs().size() > 1) 
		annealingFactor = BetaAnnealingFactor(mWeights, 0.5f);			//calculate annealing factor if more than 1 step
	for(uint i = 0; i < mWeights.size(); i++)
	{	double wa = annealingFactor * mWeights[i];
		mWeights[i] = (float)t_exp(wa);
		total += mWeights[i];											//save sum of all weights
		if(i == 0 || mWeights[i] > best)								//find highest likelihood particle
		{	best = mWeights[i];
			mBestParticle = i;
		}
	}
	mWeights *= fpType(1.0) / total;									//normalize weights
}
