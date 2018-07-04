#include "glr.h"
#include "utils.h"
#include <math.h>
/*
GLR::GLR(string name, MAB& mab, int M, int mod, double alpha, boost::mt19937& rng) : MABAlgorithm(name, mab) {
  this->ucb = (UCB1*)get_algorithm(&mab, "ucb1 ucb1", &rng);
  this->M = M;
  this->mod = mod;
  this->alpha = alpha;
  this->output_file.open("glr_output_" + to_string(this->mod) + ".txt");
  this->reset();
}

void GLR::reset() {
  mabalg_reset();
  this->ucb->reset();
  this->rewards.clear();
  this->changepoints.clear();
  this->rewards.resize(this->num_of_arms);
  this->changepoints.assign(this->num_of_arms, 0);
  this->past_k.clear();
  this->past_k.resize(this->num_of_arms);
  for (int arm = 0; arm < this->num_of_arms; arm++) {
    this->past_k[arm].assign(1,0);
  }
}

ArmPull GLR::run(vector<vector<double>>& all_pulls, int timestep, bool generate_new_pulls) {
  ArmPull armpull = this->ucb_epsilon_greedy(all_pulls, timestep, generate_new_pulls);

  // keep this->rewards updated
  this->rewards[armpull.arm_index].push_back(armpull.reward);

  this->find_changepoints(armpull.arm_index);
  this->update_UCB(armpull.arm_index);

  return armpull;
}

ArmPull GLR::ucb_epsilon_greedy(vector<vector<double>>& all_pulls, int timestep, bool generate_new_pulls) {
  // epsilon greedy exploration
  double a = random_unit();
  ArmPull armpull = ArmPull(0.,0);
  if (a < this->alpha) {
    // explore
    int arm_to_pull = rand() % this->num_of_arms;
    ArmPull new_armpull = this->pull_arm(all_pulls, timestep, generate_new_pulls, arm_to_pull);

    // update ucb
    this->ucb->num_of_pulls[arm_to_pull]++;
    this->ucb->means[arm_to_pull] = (this->ucb->means[arm_to_pull] * (this->ucb->num_of_pulls[arm_to_pull] - 1) + new_armpull.reward) / this->ucb->num_of_pulls[arm_to_pull];

    armpull.reward = new_armpull.reward;
    armpull.arm_index = new_armpull.arm_index;
  } else {
    // ucb
    ArmPull new_armpull = this->ucb->run(all_pulls, timestep, generate_new_pulls);
    armpull.reward = new_armpull.reward;
    armpull.arm_index = new_armpull.arm_index;
  }

  return armpull;
}

void GLR::find_changepoints(int arm_pulled) {
  if (this->mod == 0) { // / p_0
    if (this->rewards[arm_pulled].size() >= 2*M) {
      double mu_0 = accumulate(this->rewards[arm_pulled].begin(), this->rewards[arm_pulled].begin() + this->M, 0.) / this->M;
      double mu_1 = accumulate(this->rewards[arm_pulled].end() - this->M, this->rewards[arm_pulled].end(), 0.) / this->M;

      int num_of_pulls = this->rewards[arm_pulled].size();
      double sum_M_T = accumulate(this->rewards[arm_pulled].begin() + this->M, this->rewards[arm_pulled].end(), 0.);
      double l_m = (mu_1 - mu_0) * (sum_M_T - (num_of_pulls - this->M) * (mu_1 + mu_0) / 2);
      double prev_l = l_m;
      double highest_l = l_m;
      double lowest_l = l_m;
      int worst_k = this->M;
      int best_k = this->M;
      for (int k = this->M+1; k < num_of_pulls - this->M; k++) {
        double x_k = this->rewards[arm_pulled][k-1];
        double l_k = prev_l + (pow(mu_1, 2) - pow(mu_0,2)) / 2 - (mu_1 - mu_0) * x_k;
        if (l_k > highest_l) {
          highest_l = l_k;
          best_k = k;
        } else if (l_k < lowest_l) {
          lowest_l = l_k;
          worst_k = k;
        }
        prev_l = l_k;
      }
      int final_k = best_k;
      this->changepoints[arm_pulled] = final_k;
      this->output_file << arm_pulled << ", " << num_of_pulls << ", " << best_k << ", " << highest_l << ", " << worst_k << ", " << lowest_l << ", " << final_k << ", " << highest_l - lowest_l << endl;
      //cout << "arm " << arm_pulled << " selected " << num_of_pulls << " times and best k is " << best_k << " with likelihood " << highest_normalized_l << endl;
    }
  } else if (this->mod == 1) { // only num
    if (this->rewards[arm_pulled].size() >= 2*M) {
      double mu_0 = accumulate(this->rewards[arm_pulled].begin(), this->rewards[arm_pulled].begin() + this->M, 0.) / this->M;
      double mu_1 = accumulate(this->rewards[arm_pulled].end() - this->M, this->rewards[arm_pulled].end(), 0.) / this->M;

      int num_of_pulls = this->rewards[arm_pulled].size();
      double sum_1_M = accumulate(this->rewards[arm_pulled].begin(), this->rewards[arm_pulled].begin() + this->M, 0.);
      double sum_M_T = accumulate(this->rewards[arm_pulled].begin() + this->M, this->rewards[arm_pulled].end(), 0.);
      double l_m = 2 * (mu_0 * sum_1_M + mu_1 * sum_M_T) - this->M * pow(mu_0, 2) - (num_of_pulls - this->M) * pow(mu_1, 2);
      double prev_l = l_m;
      double highest_l = l_m;
      double lowest_l = l_m;
      int worst_k = this->M;
      int best_k = this->M;
      for (int k = this->M+1; k < num_of_pulls - this->M; k++) {
        double x_k = this->rewards[arm_pulled][k];
        double l_k = prev_l + 2*mu_0*x_k - 2*mu_1*x_k - pow(mu_0,2) + pow(mu_1,2);
        if (l_k > highest_l) {
          highest_l = l_k;
          best_k = k;
        } else if (l_k < lowest_l) {
          lowest_l = l_k;
          worst_k = k;
        }
        prev_l = l_k;
      }
      int final_k = best_k;
      this->changepoints[arm_pulled] = final_k;
      this->output_file << arm_pulled << ", " << num_of_pulls << ", " << best_k << ", " << highest_l << ", " << worst_k << ", " << lowest_l << ", " << final_k << ", " << highest_l - lowest_l << endl;
      //cout << "arm " << arm_pulled << " selected " << num_of_pulls << " times and best k is " << best_k << " with likelihood " << highest_l << endl;
    }
  } else if (this->mod == 2) { // bernoulli, / p_0
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
      int final_k = best_k;
      this->changepoints[arm_pulled] = final_k;
      this->output_file << arm_pulled << ", " << num_of_pulls << ", " << best_k << ", " << highest_l << ", " << worst_k << ", " << lowest_l << ", " << final_k << ", " << highest_l - lowest_l << endl;
      //cout << "arm " << arm_pulled << " selected " << num_of_pulls << " times and best k is " << best_k << " with likelihood " << highest_l << endl;
    }
  } else if (this->mod == 3) { // / p_1 ... / p_0
    if (this->rewards[arm_pulled].size() >= 2*M) {
      double mu_0 = accumulate(this->rewards[arm_pulled].begin(), this->rewards[arm_pulled].begin() + this->M, 0.) / this->M;
      double mu_1 = accumulate(this->rewards[arm_pulled].end() - this->M, this->rewards[arm_pulled].end(), 0.) / this->M;

      int num_of_pulls = this->rewards[arm_pulled].size();
      double sum_1_M = accumulate(this->rewards[arm_pulled].begin(), this->rewards[arm_pulled].begin() + this->M, 0.);
      double sum_M_T = accumulate(this->rewards[arm_pulled].begin() + this->M, this->rewards[arm_pulled].end(), 0.);
      double l_m = 2 * (mu_0 - mu_1) * (sum_1_M - sum_M_T) + (pow(mu_0,2) - pow(mu_1,2)) * (num_of_pulls - 2*this->M);
      double sum_1_k = sum_1_M;
      double sum_k_T = sum_M_T;
      double prev_l = l_m;
      double highest_l = l_m;
      double lowest_l = l_m;
      int worst_k = this->M;
      int best_k = this->M;
      for (int k = this->M+1; k < num_of_pulls - this->M; k++) {
        double x_k = this->rewards[arm_pulled][k];
        sum_1_k += x_k;
        sum_k_T -= x_k;
        double l_k = ((prev_l - (pow(mu_0,2) - pow(mu_1,2)) * (num_of_pulls - 2*(k-1))) / (2 * (mu_0 - mu_1)) + 2*x_k) * (2 * (mu_0 - mu_1)) + (pow(mu_0,2) - pow(mu_1,2)) * (num_of_pulls - 2*k);
        if (l_k > highest_l) {
          highest_l = l_k;
          best_k = k;
        } else if (l_k < lowest_l) {
          lowest_l = l_k;
          worst_k = k;
        }
        prev_l = l_k;
      }
      int final_k = best_k;
      this->changepoints[arm_pulled] = final_k;
      this->output_file << arm_pulled << ", " << num_of_pulls << ", " << best_k << ", " << highest_l << ", " << worst_k << ", " << lowest_l << ", " << final_k << ", " << highest_l - lowest_l << endl;
      //cout << "arm " << arm_pulled << " selected " << num_of_pulls << " times and best k is " << best_k << " with likelihood " << highest_l << endl;
    }
  } else if (this->mod == 4) { // / p_1
    if (this->rewards[arm_pulled].size() >= 2*M) {
      double mu_0 = accumulate(this->rewards[arm_pulled].begin(), this->rewards[arm_pulled].begin() + this->M, 0.) / this->M;
      double mu_1 = accumulate(this->rewards[arm_pulled].end() - this->M, this->rewards[arm_pulled].end(), 0.) / this->M;

      int num_of_pulls = this->rewards[arm_pulled].size();
      double sum_1_M = accumulate(this->rewards[arm_pulled].begin(), this->rewards[arm_pulled].begin() + this->M, 0.);
      double l_m = (mu_0 - mu_1) * (sum_1_M - this->M * (mu_0 + mu_1) / 2);
      double prev_l = l_m;
      double highest_l = l_m;
      double lowest_l = l_m;
      int best_k = this->M;
      int worst_k = this->M;
      for (int k = this->M+1; k < num_of_pulls - this->M; k++) {
        double x_k = this->rewards[arm_pulled][k];
        double l_k = prev_l - (pow(mu_0, 2) - pow(mu_1,2)) / 2 + (mu_0 - mu_1) * x_k;
        if (l_k > highest_l) {
          highest_l = l_k;
          best_k = k;
        } else if (l_k < lowest_l) {
          lowest_l = l_k;
          worst_k = k;
        }
        prev_l = l_k;
      }
      int final_k = best_k;
      this->changepoints[arm_pulled] = final_k;
      this->output_file << arm_pulled << ", " << num_of_pulls << ", " << best_k << ", " << highest_l << ", " << worst_k << ", " << lowest_l << ", " << final_k << ", " << highest_l - lowest_l << endl;
    }
  } else if (this->mod == 5) { // like glr 0 but with min k
    if (this->rewards[arm_pulled].size() >= 2*M) {
      double mu_0 = accumulate(this->rewards[arm_pulled].begin(), this->rewards[arm_pulled].begin() + this->M, 0.) / this->M;
      double mu_1 = accumulate(this->rewards[arm_pulled].end() - this->M, this->rewards[arm_pulled].end(), 0.) / this->M;

      int num_of_pulls = this->rewards[arm_pulled].size();
      double sum_M_T = accumulate(this->rewards[arm_pulled].begin() + this->M, this->rewards[arm_pulled].end(), 0.);
      double l_m = (mu_1 - mu_0) * (sum_M_T - (num_of_pulls - this->M) * (mu_1 + mu_0) / 2);
      double prev_l = l_m;
      double highest_l = l_m;
      double lowest_l = l_m;
      int worst_k = this->M;
      int best_k = this->M;
      for (int k = this->M+1; k < num_of_pulls - this->M; k++) {
        double x_k = this->rewards[arm_pulled][k-1];
        double l_k = prev_l + (pow(mu_1, 2) - pow(mu_0,2)) / 2 - (mu_1 - mu_0) * x_k;
        if (l_k > highest_l) {
          highest_l = l_k;
          best_k = k;
        } else if (l_k < lowest_l) {
          lowest_l = l_k;
          worst_k = k;
        }
        prev_l = l_k;
      }
      int final_k = min(best_k, worst_k);
      this->changepoints[arm_pulled] = final_k;
      this->output_file << arm_pulled << ", " << num_of_pulls << ", " << best_k << ", " << highest_l << ", " << worst_k << ", " << lowest_l << ", " << final_k << ", " << highest_l - lowest_l << endl;
    }
  } else if (this->mod == 6) { // use min of window of last best_k
    if (this->rewards[arm_pulled].size() >= 2*M) {
      double mu_0 = accumulate(this->rewards[arm_pulled].begin(), this->rewards[arm_pulled].begin() + this->M, 0.) / this->M;
      double mu_1 = accumulate(this->rewards[arm_pulled].end() - this->M, this->rewards[arm_pulled].end(), 0.) / this->M;

      int num_of_pulls = this->rewards[arm_pulled].size();
      double sum_M_T = accumulate(this->rewards[arm_pulled].begin() + this->M, this->rewards[arm_pulled].end(), 0.);
      double l_m = (mu_1 - mu_0) * (sum_M_T - (num_of_pulls - this->M) * (mu_1 + mu_0) / 2);
      double prev_l = l_m;
      double highest_l = l_m;
      double lowest_l = l_m;
      int worst_k = this->M;
      int best_k = this->M;
      for (int k = this->M+1; k < num_of_pulls - this->M; k++) {
        double x_k = this->rewards[arm_pulled][k-1];
        double l_k = prev_l + (pow(mu_1, 2) - pow(mu_0,2)) / 2 - (mu_1 - mu_0) * x_k;
        if (l_k > highest_l) {
          highest_l = l_k;
          best_k = k;
        } else if (l_k < lowest_l) {
          lowest_l = l_k;
          worst_k = k;
        }
        prev_l = l_k;
      }

      this->past_k[arm_pulled].push_back(best_k);
      this->past_k[arm_pulled].erase(this->past_k[arm_pulled].begin()); // cycle
      int final_k = this->past_k[arm_pulled][0];
      for (int i = 1; i < this->past_k[arm_pulled].size(); i++) {
        final_k = min(final_k, this->past_k[arm_pulled][i]);
      }

      this->changepoints[arm_pulled] = final_k;
      this->output_file << arm_pulled << ", " << num_of_pulls << ", " << best_k << ", " << highest_l << ", " << worst_k << ", " << lowest_l << ", " << final_k << ", " << highest_l - lowest_l << endl;
    }
  }
}

void GLR::update_UCB(int arm_pulled) {
  this->ucb->num_of_pulls[arm_pulled] = this->rewards[arm_pulled].size() - this->changepoints[arm_pulled];
  this->ucb->means[arm_pulled] = accumulate(this->rewards[arm_pulled].begin() + this->changepoints[arm_pulled], this->rewards[arm_pulled].end(), 0.) / this->ucb->num_of_pulls[arm_pulled];

}
*/
