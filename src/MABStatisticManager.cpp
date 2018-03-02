#include "MABStatisticManager.h"
#include <fstream>
#include <stdlib.h>

MABStatisticManager::MABStatisticManager(MAB* mab, vector<MABAlgorithm*>* algs) {
  this->mab = mab;
  this->algs = *algs;
  this->reset();
}


void MABStatisticManager::analyze_pulls(vector<vector<double>>& pulls) {
  // pulls[timestep][arm]
  int total_timesteps = pulls.size();

  // Fill best_arm vector
  if (*(this->mab->mabtype) == MABType::Stochastic) {
    for (int t = 0; t < total_timesteps; t++) {
  		double cur_best_mean = -10000; // TODO: substitute -10000 with -INF
      int best_arm = -1;
  		for (int a = 0; a < this->mab->arms.size(); a++) {
        double arm_mean = this->mab->arms[a]->get_mean(t);
        if (arm_mean > cur_best_mean) {
          cur_best_mean = arm_mean;
          best_arm = a;
        }
  		}
  		this->best_arm.push_back(best_arm);
  	}
  } else {
    int best_arm = -1;
		double max_sum = -10000;
		for (int a = 0; a < this->mab->arms.size(); a++) {
      double sum_of_elems = 0;
      for (int t = 0; t < total_timesteps; t++) {
        sum_of_elems += pulls[t][a];
      }
			if (sum_of_elems > max_sum) {
				max_sum = sum_of_elems;
				best_arm = a;
			}
		}
    this->best_arm.assign(total_timesteps, best_arm);
  }
}

void MABStatisticManager::update(ArmPull armpull, int alg_index, int timestep, vector<double> pulls) {
  if (*(this->mab->mabtype) == MABType::Stochastic) {
    double best_mean = this->mab->arms[this->best_arm[timestep]]->get_mean(timestep);
    double sampled_arm_mean = this->mab->arms[armpull.arm_index]->get_mean(timestep);
    this->regrets[alg_index].push_back(best_mean - sampled_arm_mean);
  } else {
    double best_reward = pulls[this->best_arm[timestep]];
    double sampled_reward = pulls[armpull.arm_index];
    this->regrets[alg_index].push_back(best_reward - sampled_reward);
  }
}

void MABStatisticManager::reset() {
  this->best_arm.clear();
  this->regrets.clear();
  this->regrets.resize(this->algs.size());
}


void MABStatisticManager::write_distributions() {
	ofstream outputFile("temp/distributions_" + to_string(this->mab->get_id()) + ".txt");

	for (auto d : this->mab->arms) {
		outputFile << d->toFile() << endl;
	}
}

void MABStatisticManager::write_regrets(bool should_append) {
	ofstream outputFile("temp/regrets_" + to_string(this->mab->get_id()) + ".txt", should_append ? ofstream::app : ofstream::out);
	int write_every = 5;

	if (!should_append)
		outputFile << write_every << endl;

	for (int alg_index = 0; alg_index < this->algs.size(); alg_index++) {
		outputFile << this->algs[alg_index]->name << " ";

		double cum_regret = 0;
		for (int j = 0; j < this->regrets[alg_index].size(); j++) {
			if (j % write_every == 0) {
				outputFile << (cum_regret + this->regrets[alg_index][j]) << " ";
				cum_regret = 0;
			} else {
				cum_regret += this->regrets[alg_index][j];
			}
		}

		outputFile << endl;
	}
}
