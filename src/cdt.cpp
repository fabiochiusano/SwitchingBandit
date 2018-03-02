#include "cdt.h"
#include <algorithm>

CDT_PH::CDT_PH(double gamma, double lambda) {
  this->gamma = gamma;
  this->lambda = lambda;
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
