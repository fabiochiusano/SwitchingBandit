#include "cd_algorithm.h"
#include "utils.h"
#include "ucb.h"

CD_Algorithm::CD_Algorithm(string name, MAB& mab, string cdt_line, string sub_alg_line, bool use_history, double alpha, boost::mt19937& rng) : MABAlgorithm(name, mab) {
  this->alg = get_algorithm(&mab, sub_alg_line, &rng);
  for (int i = 0; i < this->num_of_arms; i++) {
    this->cdts.push_back(get_cdt(cdt_line));
  }
  this->use_history = use_history;
  this->alpha = alpha;
  this->reset();
}

void CD_Algorithm::reset() {
  mabalg_reset();
  this->alg->reset();
  for (int i = 0; i < this->num_of_arms; i++) {
    this->cdts[i]->reset();
  }
  this->chosen_arms.clear();
  this->last_resets_global.assign(this->num_of_arms, -1);
}

ArmPull CD_Algorithm::run(vector<vector<double>>& all_pulls, int timestep, bool generate_new_pulls) {
  vector<double> pulls = all_pulls[timestep];

  // epsilon greedy exploration
  double a = random_unit();
  ArmPull armpull = ArmPull(0.,0);
  if (a < this->alpha) {
    // explore
    int arm_to_pull = rand() % this->num_of_arms;
    ArmPull new_armpull = this->pull_arm(all_pulls, timestep, generate_new_pulls, arm_to_pull);

    // update ucb
    UCB1* ucb = (UCB1*)(this->alg);
    ucb->num_of_pulls[arm_to_pull]++;
    ucb->means[arm_to_pull] = (ucb->means[arm_to_pull] * (ucb->num_of_pulls[arm_to_pull] - 1) + new_armpull.reward) / ucb->num_of_pulls[arm_to_pull];

    armpull.reward = new_armpull.reward;
    armpull.arm_index = new_armpull.arm_index;
  } else {
    // ucb
    ArmPull new_armpull = this->alg->run(all_pulls, timestep, generate_new_pulls);
    armpull.reward = new_armpull.reward;
    armpull.arm_index = new_armpull.arm_index;
  }

  this->chosen_arms.push_back(armpull.arm_index);
  CDT_Result cdt_result = this->cdts[armpull.arm_index]->run(armpull.reward);
  if (cdt_result.alarm) {
    //cout << this->name << ": alarm raised at timestep " << timestep << " on arm " << armpull.arm_index << endl;

    this->update_history_UCB(all_pulls, timestep, armpull.arm_index, cdt_result);

    this->cdts[armpull.arm_index]->reset();
  }
  return armpull;
}

void CD_Algorithm::update_history_UCB(vector<vector<double>>& all_pulls, int timestep, int arm_reset, CDT_Result cdt_result) {
  UCB1* ucb = (UCB1*)(this->alg);

  ucb->num_of_pulls[arm_reset] = 0;
  ucb->means[arm_reset] = 0;

  if (this->use_history) {
    int arm_counter = 0;
    int change_estimate_global = 0;
    int history_amount = 0;
    bool set_estimate = false;
    for (int i = this->last_resets_global[arm_reset]+1; i < this->chosen_arms.size(); i++) {
      if (this->chosen_arms[i] == arm_reset) {
        arm_counter++;
        if (arm_counter > cdt_result.change_estimate) {
          history_amount++;
        }
      }

      if (arm_counter == cdt_result.change_estimate && !set_estimate) {
        change_estimate_global = i;
        set_estimate = true;
      }
    }

    //cout << "global estimated change time: " << change_estimate_global << ", history: " << history_amount << endl;

    double mean = 0;
    for (int i = change_estimate_global+1; i < timestep; i++) {
      int pulled_arm = this->chosen_arms[i];
      if (pulled_arm == arm_reset) {
        ucb->num_of_pulls[arm_reset]++;
        double reward = all_pulls[i][pulled_arm];
        mean += reward;
      }
    }
    ucb->means[arm_reset] = mean/max(ucb->num_of_pulls[arm_reset],1);
    this->last_resets_global[arm_reset] = change_estimate_global;
  }
}
