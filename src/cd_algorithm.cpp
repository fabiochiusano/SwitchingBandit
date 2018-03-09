#include "cd_algorithm.h"
#include "utils.h"

CD_Algorithm::CD_Algorithm(string name, MAB& mab, string cdt_line, string sub_alg_line, boost::mt19937& rng) : MABAlgorithm(name, mab) {
  this->alg = get_algorithm(&mab, sub_alg_line, &rng);
  for (int i = 0; i < this->num_of_arms; i++) {
    this->cdts.push_back(get_cdt(cdt_line));
  }
  this->reset();
}

void CD_Algorithm::reset() {
  mabalg_reset();
  this->alg->reset();
  for (int i = 0; i < this->num_of_arms; i++) {
    this->cdts[i]->reset();
  }
  this->timestep = 0;
}

ArmPull CD_Algorithm::run(vector<double>& pulls, bool generate_new_pulls) {
  this->timestep++;
  ArmPull armpull = this->alg->run(pulls, generate_new_pulls);
  bool alarm_raised = this->cdts[armpull.arm_index]->run(armpull.reward);
  if (alarm_raised) {
    cout << this->name << ": alarm raised at timestep " << this->timestep << endl;
    mabalg_reset();
    this->alg->reset();
    for (int i = 0; i < this->num_of_arms; i++) {
      this->cdts[i]->reset();
    }
  }
  return armpull;
}
