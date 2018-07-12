#include "statistic_manager.h"
#include <fstream>
#include <stdlib.h>
#include "utils.h"

StatisticManager::StatisticManager(string name, MAB* mab, vector<MABAlgorithm*>& algs) {
  this->name = name;
  this->mab = mab;
  this->algs = algs;
  this->reset();
}

void StatisticManager::set_mab_type(RegretType mabtype) {
  this->mabtype = mabtype;
}


void StatisticManager::analyze_pulls(vector<vector<double>>& pulls) {
  // pulls[timestep][arm]
  int total_timesteps = pulls.size();

  MABExperiment* mabexp = dynamic_cast<MABExperiment*>(this->mab);
  if (mabexp == NULL) {
    cout << "Calling StatisticManager::analyze_pulls on a not MABExperiment!" << endl;
    return;
  }

  this->pulls = pulls;

  // Fill best_arm vector
  if (this->mabtype == RegretType::Stochastic) {
    for (int t = 0; t < total_timesteps; t++) {
  		double cur_best_mean = -10000; // TODO: substitute -10000 with -INF
      int best_arm = -1;
  		for (int a = 0; a < mabexp->arms.size(); a++) {
        double arm_mean = mabexp->arms[a]->get_mean(t);
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
		for (int a = 0; a < mabexp->arms.size(); a++) {
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

  // Resize vectors
  for (int i = 0; i < this->algs.size(); i++) {
    this->regrets[i].resize(total_timesteps);
    this->rewards[i].resize(total_timesteps);
    this->chosen_arm[i].resize(total_timesteps);
  }
}

void StatisticManager::update(int arm_to_pull, double reward, int alg_index, int timestep) {
  MABExperiment* mabexp = dynamic_cast<MABExperiment*>(this->mab);
  // regret
  if (mabexp != NULL) {
    if (this->mabtype == RegretType::Stochastic) {
      double best_mean = mabexp->arms[this->best_arm[timestep]]->get_mean(timestep);
      double sampled_arm_mean = mabexp->arms[arm_to_pull]->get_mean(timestep);
      this->regrets[alg_index][timestep] = best_mean - sampled_arm_mean;
    } else {
      double best_reward = this->pulls[timestep][this->best_arm[timestep]];
      double sampled_reward = this->pulls[timestep][arm_to_pull];
      this->regrets[alg_index][timestep] = best_reward - sampled_reward;
    }
  }

  // rewards
  this->rewards[alg_index][timestep] = reward;

  // chosen arm
  this->chosen_arm[alg_index][timestep] = arm_to_pull;
}

void StatisticManager::reset() {
  this->best_arm.clear();
  this->regrets.clear();
  this->regrets.resize(this->algs.size());
  this->rewards.clear();
  this->rewards.resize(this->algs.size());
  this->chosen_arm.clear();
  this->chosen_arm.resize(this->algs.size());
  this->pulls.clear();
}


void StatisticManager::write_distributions() {
  make_dir("temp/" + this->name);
	ofstream outputFile("temp/" + this->name + "/distributions.txt");

  MABExperiment* mabexp = dynamic_cast<MABExperiment*>(this->mab);
  if (mabexp == NULL) {
    cout << "Calling StatisticManager::write_distributions on a not MABExperiment!" << endl;
    return;
  }

	for (auto d : mabexp->arms) {
		outputFile << d->toFile() << endl;
	}
}

void StatisticManager::write_regrets(int sim) {
  make_dir("temp/" + this->name + "/regrets");
	ofstream outputFile("temp/" + this->name + "/regrets/" + to_string(sim) + ".txt");
	int write_every = 5; // TODO: move this somewhere else
  outputFile << write_every << endl;

	for (int alg_index = 0; alg_index < this->algs.size(); alg_index++) {
		outputFile << this->algs[alg_index]->name << " ";

		double cum_regret = 0;
		for (int j = 0; j < this->regrets[alg_index].size(); j++) {
			if ((j + 1) % write_every == 0) {
				outputFile << (cum_regret + this->regrets[alg_index][j]) << " ";
				cum_regret = 0;
			} else {
				cum_regret += this->regrets[alg_index][j];
			}
		}

		outputFile << endl;
	}
}

void StatisticManager::write_rewards(int sim) {
  make_dir("temp/" + this->name + "/rewards");
  ofstream outputFile("temp/" + this->name + "/rewards/" + to_string(sim) + ".txt");
  int write_every = 5; // TODO: move this somewhere else
  outputFile << write_every << endl;

  for (int alg_index = 0; alg_index < this->algs.size(); alg_index++) {
		outputFile << this->algs[alg_index]->name << " ";

    double cum_reward = 0;
		for (int j = 0; j < this->rewards[alg_index].size(); j++) {
			if ((j + 1) % write_every == 0) {
				outputFile << (cum_reward + this->rewards[alg_index][j]) << " ";
				cum_reward = 0;
			} else {
				cum_reward += this->rewards[alg_index][j];
			}
		}

		outputFile << endl;
	}
}

void StatisticManager::write_chosen_arm(int sim) {
  make_dir("temp/" + this->name + "/chosen_arm");
  ofstream outputFile("temp/" + this->name + "/chosen_arm/" + to_string(sim) + ".txt");

  for (int alg_index = 0; alg_index < this->algs.size(); alg_index++) {
		outputFile << this->algs[alg_index]->name << " ";
		for (int j = 0; j < this->rewards[alg_index].size(); j++) {
			outputFile << this->chosen_arm[alg_index][j] << " ";
		}

		outputFile << endl;
	}
}
