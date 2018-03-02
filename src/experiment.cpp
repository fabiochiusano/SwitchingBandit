#include "experiment.h"
#include "MABStatisticManager.h"

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
  MABStatisticManager mab_manager(this->mab, &(this->algs));

  mab_manager.write_distributions();

  for (int cur_simulation = 1; cur_simulation <= this->num_simulations; cur_simulation++) {
    // Should generate all the pulls because they are needed in advice in an adversary environment
    vector<vector<double>> pulls = this->mab->generate_pulls(this->timesteps); // pulls[timestep][arm]

    mab_manager.analyze_pulls(pulls);

		for (int timestep = 0; timestep < this->timesteps; timestep++) {
      for (int alg_index = 0; alg_index < this->algs.size(); alg_index++) {
        bool generate_new_pulls = false;

        // The generated armpull is relative to the main mab (not to any meta-mab)
        ArmPull armpull = this->algs[alg_index]->run(pulls[timestep], generate_new_pulls);

        mab_manager.update(armpull, alg_index, timestep, pulls[timestep]);
      }
		}

		mab_manager.write_regrets(cur_simulation != 1);

    for (auto alg : this->algs) {
  		alg->reset();
  	}
    mab_manager.reset();
	}
}
