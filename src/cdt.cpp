#include "cdt.h"
#include <algorithm>

CDT_PH::CDT_PH(double gamma, double lambda) {
  this->gamma = gamma;
  this->lambda = lambda;
  this->reset();
}

Two_Sided_CUSUM::Two_Sided_CUSUM(int M, double epsilon, double threshold) {
  this->M = M;
  this->epsilon = epsilon;
  this->threshold = threshold;
  this->reset();
}

CDT_PH_RHO::CDT_PH_RHO(double gamma, double lambda, double rho) {
  this->gamma = gamma;
  this->lambda = lambda;
  this->rho = rho;
  this->reset();
}


void CDT_PH::reset() {
  this->PH = 0;
  this->mean_reward = 0;
  this->num_rewards = 0;
}

void CDT_PH_RHO::reset() {
  this->PH = 0;
  this->mean_reward = 0;
  this->num_rewards = 0;
}

void Two_Sided_CUSUM::reset() {
  this->mean_over_M = 0;
  this->g_minus = 0;
  this->g_plus = 0;
  this->num_rewards = 0;
}


bool CDT_PH::run(double reward) {
  // Update reward mean
  this->num_rewards++;
  this->mean_reward = (this->mean_reward * (this->num_rewards - 1) + reward) / this->num_rewards;

  // Update PH statistic
  this->PH = max(this->PH - reward + this->mean_reward - this->gamma, 0.);

  // Return alarm
  return this->PH > this->lambda;
}

bool CDT_PH_RHO::run(double reward) {
  // Update reward mean
  this->num_rewards++;
  this->mean_reward = (this->mean_reward * (this->num_rewards - 1) + reward) / this->num_rewards;

  // Update PH statistic
  this->PH = max((this->rho * this->PH) - reward + this->mean_reward - this->gamma, 0.);

  // Return alarm
  return this->PH > this->lambda;
}

bool Two_Sided_CUSUM::run(double reward) {
  // Update reward mean
  this->num_rewards++;
  if (num_rewards <= this->M) {
    this->mean_over_M += reward;
  }
  if (num_rewards == this->M) {
    this->mean_over_M /= this->M;
  }
  if (num_rewards <= this->M) {
    return false;
  } else {
    double s_plus = reward - this->mean_over_M - this->epsilon;
    double s_minus = this->mean_over_M - reward  - epsilon;

    this->g_plus = max(0., g_plus + s_plus);
    this->g_minus = max(0., g_minus + s_minus);

    return this->g_plus > this->threshold || this->g_minus > this->threshold;
  }
}
