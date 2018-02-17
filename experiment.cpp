#include "experiment.h"

Experiment::Experiment(int num_simulations, int timesteps, int seed) {
  this->num_simulations = num_simulations;
  this->timesteps = timesteps;
  this->seed = seed;
}

void Experiment::set_mab(MAB* mab) {
  this->mab = mab;
}

void Experiment::add_alg(MABAlgorithm* alg) {
  this->algs.push_back(alg);
}

void Experiment::run() {
  this->mab->write_distributions();
	this->mab->write_means_in_time(this->timesteps);

  for (int cur_simulation = 1; cur_simulation <= this->num_simulations; cur_simulation++) {
		for (int timestep = 0; timestep < this->timesteps; timestep++) {
      this->mab->run(this->algs, timestep, this->timesteps);
		}

		if (cur_simulation == 1) {
			mab->write_regrets(algs, false);
		} else {
			mab->write_regrets(algs, true);
		}

		mab->reset(algs);
	}
}
