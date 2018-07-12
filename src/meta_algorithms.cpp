#include "utils.h"
#include "ucb.h"
#include "meta_algorithms.h"
#include <fstream>

Round_Algorithm::Round_Algorithm(string name, int num_of_arms, string sub_alg_line, boost::mt19937& rng) : MABAlgorithm(name, num_of_arms) {
  this->sub_alg = get_algorithm(sub_alg_line, num_of_arms, &rng);
  this->reset(-1);
}

void Round_Algorithm::reset(int action) {
	this->MABAlgorithm::reset(action);
  if (action == -1) {
    this->arms_pulled.assign(this->num_of_arms, false);
  } else {
    this->arms_pulled[action] = false;
  }
  this->sub_alg->reset(action);
}

int Round_Algorithm::choose_action() {
	int arm_to_pull = -1;
  for (int i = 0; i < this->arms_pulled.size(); i++) {
    if (!this->arms_pulled[i]) {
      arm_to_pull = i;
      break;
    }
  }
	if (arm_to_pull == -1) {
		// Delegate to sub_alg
		arm_to_pull = this->sub_alg->choose_action();
	}
	return arm_to_pull;
}

void Round_Algorithm::receive_reward(double reward, int pulled_arm) {
  this->MABAlgorithm::receive_reward(reward, pulled_arm);
  this->arms_pulled[pulled_arm] = true;
	this->sub_alg->receive_reward(reward, pulled_arm);
}




Algorithm_With_Uniform_Exploration::Algorithm_With_Uniform_Exploration(string name, int num_of_arms, string sub_alg_line, double alpha, boost::mt19937& rng) : MABAlgorithm(name, num_of_arms) {
	this->sub_alg = get_algorithm(sub_alg_line, num_of_arms, &rng);
	this->alpha = alpha;
}

void Algorithm_With_Uniform_Exploration::reset(int action) {
	this->MABAlgorithm::reset(action);
  this->sub_alg->reset(action);
}

int Algorithm_With_Uniform_Exploration::choose_action() {
	int arm_to_pull = -1;
	float r = random_unit();
	if (r < this->alpha) { // explore
		arm_to_pull = rand() % this->num_of_arms;
	} else {
		arm_to_pull = this->sub_alg->choose_action();
	}
	return arm_to_pull;
}

void Algorithm_With_Uniform_Exploration::receive_reward(double reward, int pulled_arm) {
  this->MABAlgorithm::receive_reward(reward, pulled_arm);
	this->sub_alg->receive_reward(reward, pulled_arm);
}



CD_Algorithm::CD_Algorithm(string name, int num_of_arms, int M, string cdt_line, string sub_alg_line, bool use_history, boost::mt19937& rng) : MABAlgorithm(name, num_of_arms) {
  this->M = M;
  this->sub_alg = get_algorithm(sub_alg_line, num_of_arms, &rng);
  for (int i = 0; i < num_of_arms; i++) {
    this->cdts.push_back(get_cdt(cdt_line));
  }
  this->use_history = use_history;
  this->reset(-1);
}

void CD_Algorithm::reset(int action) {
  this->MABAlgorithm::reset(action);
  this->sub_alg->reset(action);
  if (action == -1) {
    for (int i = 0; i < this->num_of_arms; i++) {
      this->cdts[i]->reset();
    }
    this->till_M.clear();
    this->till_M.assign(this->num_of_arms, this->M);
    this->collected_rewards.clear();
    this->collected_rewards.resize(this->num_of_arms);
    this->timestep = 0;
  } else {
    this->cdts[action]->reset();
    this->till_M[action] = this->M;
    this->collected_rewards[action].clear();
  }
}

int CD_Algorithm::choose_action() {
  int action = -1;
  for (int i = 0; i < this->num_of_arms; i++) {
    if (this->till_M[i] > 0) {
      action = i;
      this->till_M[i]--;
      break;
    }
  }
  if (action == -1) {
    action = this->sub_alg->choose_action();
  }
  return action;
}

void CD_Algorithm::receive_reward(double reward, int pulled_arm) {
  this->MABAlgorithm::receive_reward(reward, pulled_arm);
  this->sub_alg->receive_reward(reward, pulled_arm);
  this->collected_rewards[pulled_arm].push_back(reward);

  CDT_Result cdt_result = this->cdts[pulled_arm]->run(reward);
  if (cdt_result.alarm) {
    // Log alarm
    ofstream cdt_file("temp/cdt.txt", fstream::app);
    cdt_file << this->name << " ";

    this->sub_alg->reset(pulled_arm);
    if (this->use_history) {
      for (int i = cdt_result.change_estimate + 1; i < this->collected_rewards[pulled_arm].size(); i++) {
        this->sub_alg->receive_reward(this->collected_rewards[pulled_arm][i], pulled_arm);
      }
    }
    this->collected_rewards[pulled_arm].clear();
    this->cdts[pulled_arm]->reset();
    this->till_M[pulled_arm] = this->M;
  }
  this->timestep++;
}

ADAPT_EVE::ADAPT_EVE(string name, int num_of_arms, int meta_duration, string cdt_line, string sub_alg_line, boost::mt19937& rng) : MABAlgorithm(name, num_of_arms) {

  this->meta_duration = meta_duration;
  //this->sub_alg_line = sub_alg_line;
  this->rng = &rng;

  for (int i = 0; i < this->num_of_arms; i++) {
    this->cdts.push_back(get_cdt(cdt_line));
  }

  this->core_sub_alg = get_algorithm(sub_alg_line, num_of_arms, this->rng);
  this->other_sub_alg = get_algorithm(sub_alg_line, num_of_arms, this->rng);

  this->meta_alg = get_algorithm(sub_alg_line, 2, this->rng);

  this->reset(-1);
}

void ADAPT_EVE::reset(int action) {
	this->MABAlgorithm::reset(action);
  if (action == -1) {
    for (int i = 0; i < this->num_of_arms; i++) {
      this->cdts[i]->reset();
    }
  } else {
    this->cdts[action]->reset();
  }

  this->core_sub_alg->reset(action);
  this->other_sub_alg->reset(action);
  this->meta_alg->reset(action);

  this->is_meta = false;
  this->t_meta = 0;

  this->timestep = 0;
}

int ADAPT_EVE::choose_action() {
  int action = -1;
  if (!this->is_meta) {
    action = this->core_sub_alg->choose_action();
  } else {
    this->saved_meta_action = this->meta_alg->choose_action();
    if (this->saved_meta_action == 0) {
      action = this->core_sub_alg->choose_action();
    } else {
      action = this->other_sub_alg->choose_action();
    }
  }
  return action;
}

void ADAPT_EVE::receive_reward(double reward, int pulled_arm) {
  this->MABAlgorithm::receive_reward(reward, pulled_arm);
  this->timestep++;

  if (!this->is_meta) {
    this->core_sub_alg->receive_reward(reward, pulled_arm);

    CDT_Result cdt_result = this->cdts[pulled_arm]->run(reward);
    if (cdt_result.alarm) {
      cout << this->name << ": alarm raised at timestep " << this->timestep << endl;

      this->is_meta = true;
      this->t_meta = 0;

      this->other_sub_alg->reset(-1);
      this->meta_alg->reset(-1);
    }
  } else {
    t_meta++;

    this->meta_alg->receive_reward(reward, this->saved_meta_action);
    if (this->saved_meta_action == 0) {
      this->core_sub_alg->receive_reward(reward, pulled_arm);
    } else {
      this->other_sub_alg->receive_reward(reward, pulled_arm);
    }

    if (t_meta == this->meta_duration) {
      // Time to decide which algorithm has been the best, i.e. the one that has been selected
      // the most by the meta algorithm // TODO: is the selection of the best sub_alg in adapt_eve ok?
      int core_pulls = this->meta_alg->num_of_pulls[0];
      int other_pulls = this->meta_alg->num_of_pulls[1];

      if (other_pulls > core_pulls) {
        // Swap core_alg and other_alg
        MABAlgorithm *temp = this->core_sub_alg;
        this->core_sub_alg = this->other_sub_alg;
        this->other_sub_alg = temp;

        cout << "core pulls: " << core_pulls << ", other pulls: " << other_pulls << ", keeping new alg" << endl;
      } else {
        cout << "core pulls: " << core_pulls << ", other pulls: " << other_pulls << ", keeping old alg" << endl;
      }

      this->is_meta = false;

      // Restart CDTs
      for (int i = 0; i < this->num_of_arms; i++) {
        this->cdts[i]->reset();
      }
    }
  }
}

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
