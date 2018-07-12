#include "glr.h"
#include "utils.h"
#include <math.h>
/*
GLR::GLR(string name, int num_of_arms, int M, string sub_alg_line, boost::mt19937& rng) : MABAlgorithm(name, num_of_arms) {
  this->sub_alg = get_algorithm(sub_alg_line, num_of_arms, &rng);
  this->M = M;
  this->reset();
}

void GLR::reset(int action) {
  this->MABAlgorithm::reset(action);
  this->sub_alg->reset(action);
  if (action == -1) {
    this->rewards.clear();
    this->changepoints.clear();
    this->rewards.resize(this->num_of_arms);
    this->changepoints.assign(this->num_of_arms, 0);
    this->past_k.clear();
    this->past_k.resize(this->num_of_arms);
    for (int arm = 0; arm < this->num_of_arms; arm++) {
      this->past_k[arm].assign(1, 0);
    }
  } else {
    this->rewards[action].clear();
    this->changepoints[action] = 0;
    this->past_k[action].clear();
  }
}

int GLR::choose_action() {
  return this->sub_alg->choose_action();
}

void GLR::receive_reward(double reward, int pulled_arm) {
  this->rewards[pulled_arm].push_back(reward);
  this->find_changepoints(pulled_arm);
  this->update_sub_alg(pulled_arm);
}

void GLR::find_changepoints(int arm_pulled) {
  if (this->rewards[arm_pulled].size() >= 2*M) {
      double p_0 = accumulate(this->rewards[arm_pulled].begin(), this->rewards[arm_pulled].begin() + this->M, 0.) / this->M;
      double p_1 = accumulate(this->rewards[arm_pulled].end() - this->M, this->rewards[arm_pulled].end(), 0.) / this->M;

      int num_of_pulls = this->rewards[arm_pulled].size();
      double sum_x_i = accumulate(this->rewards[arm_pulled].begin() + this->M, this->rewards[arm_pulled].end(), 0.);
      double l_m = sum_x_i * log(p_1/p_0) + (num_of_pulls - this->M - sum_x_i) * log((1 - p_1)/(1 - p_0));

      double prev_l = l_m;
      double highest_l = l_m;
      double lowest_l = l_m;
      int worst_k = this->M;
      int best_k = this->M;
      for (int k = this->M+1; k < num_of_pulls - this->M; k++) {
        double x_k = this->rewards[arm_pulled][k-1];
        double l_k = prev_l;
        if (x_k > 0.5) { // i.e. = 1
          l_k -= log(p_1 / p_0);
        } else {
          l_k -= log((1 - p_1)/(1 - p_0));
        }
        if (l_k > highest_l) {
          highest_l = l_k;
          best_k = k;
        } else if (l_k < lowest_l) {
          lowest_l = l_k;
          worst_k = k;
        }
        prev_l = l_k;
      }
      this->changepoints[arm_pulled] = best_k;
  }
}

void GLR::update_sub_alg(int arm_pulled) {
  this->sub_alg->reset(arm_pulled);
  for (int i = this->changepoints[arm_pulled] + 1; i < this->rewards[arm_pulled].size(); i++) {
    this->sub_alg->receive_reward(this->rewards[arm_pulled][i], arm_pulled);
  }
}
*/
