#include "utils.h"
#include "ucb.h"
#include "meta_algorithms.h"
#include <fstream>
#include <algorithm>    // std::sort

MetaAlgorithm::MetaAlgorithm(string name, int num_of_arms) : MABAlgorithm(name, num_of_arms) {}

MABAlgorithm* MetaAlgorithm::get_sub_alg() {
  MetaAlgorithm* b = dynamic_cast<MetaAlgorithm*>(this->sub_alg);
  if (b == NULL) { // if not meta
    return this->sub_alg;
  } else { // if meta
    return b->get_sub_alg();
  }
}

Round_Algorithm::Round_Algorithm(string name, int num_of_arms, string sub_alg_line, boost::mt19937& rng, MAB* mab) : MetaAlgorithm(name, num_of_arms) {
  this->sub_alg = get_algorithm(sub_alg_line, &rng, mab);
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




Algorithm_With_Uniform_Exploration::Algorithm_With_Uniform_Exploration(string name, int num_of_arms, string sub_alg_line, double alpha, boost::mt19937& rng, MAB* mab) : MetaAlgorithm(name, num_of_arms) {
	this->sub_alg = get_algorithm(sub_alg_line, &rng, mab);
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



CD_Algorithm::CD_Algorithm(string name, int num_of_arms, int M, string cdt_line, string sub_alg_line, bool use_history, int max_history, bool smart_resets, boost::mt19937& rng, MAB* mab) : MetaAlgorithm(name, num_of_arms) {
  this->M = M;
  this->max_history = max_history;
  this->sub_alg = get_algorithm(sub_alg_line, &rng, mab);
  for (int i = 0; i < num_of_arms; i++) {
    this->cdts_up.push_back(get_cdt(cdt_line + " 1"));
    this->cdts_down.push_back(get_cdt(cdt_line + " 0"));
    if (smart_resets) {
      this->sr_cdts_up.push_back(get_cdt(cdt_line + " 1"));
      this->sr_cdts_down.push_back(get_cdt(cdt_line + " 0"));
    }
  }
  this->use_history = use_history;
  this->smart_resets = smart_resets;
  if (smart_resets) {
    this->sr_change_estimates.assign(num_of_arms, 0);
  }
  this->last_resets_up.assign(num_of_arms, false);
  //this->num_alarms_up = 0;
  //this->num_alarms_down = 0;
  this->reset(-1);
}

void CD_Algorithm::reset(int action) {
  this->MABAlgorithm::reset(action);
  this->sub_alg->reset(action);
  if (action == -1) {
    for (int i = 0; i < this->num_of_arms; i++) {
      this->cdts_up[i]->reset(0);
      this->cdts_down[i]->reset(0);
      if (this->smart_resets) {
        this->sr_cdts_up[i]->reset(0);
        this->sr_cdts_down[i]->reset(0);
      }
    }
    this->till_M.clear();
    this->till_M.assign(this->num_of_arms, this->M);
    this->collected_rewards.clear();
    this->collected_rewards.resize(this->num_of_arms);
    if (this->smart_resets) {
      this->sr_change_estimates.assign(num_of_arms, 0);
      this->sr_collected_rewards.clear();
      this->sr_collected_rewards.resize(this->num_of_arms);
    }
    this->last_resets_up.assign(num_of_arms, false);
    this->timestep = 0;
    this->to_be_reset.clear();
  } else {
    this->cdts_up[action]->reset(0);
    this->cdts_down[action]->reset(0);
    this->till_M[action] = this->M;
    this->collected_rewards[action].clear();
    if (this->smart_resets) {
      this->sr_cdts_up[action]->reset(0);
      this->sr_cdts_down[action]->reset(0);
      this->sr_change_estimates[action] = 0;
      this->sr_collected_rewards[action].clear();
    }
    this->last_resets_up[action] = false;
    this->to_be_reset.erase(action);
  }
  //cout << "Alarms up: " << this->num_alarms_up / 100.0 << endl;
  //cout << "Alarms down: " << this->num_alarms_down / 100.0 << endl;
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

void CD_Algorithm::reset_arm(int pulled_arm) {
  this->sub_alg->reset(pulled_arm);
  this->MABAlgorithm::reset(pulled_arm);
  if (this->use_history) {
    int change_estimate = 0;
    bool alarm_up = this->last_resets_up[pulled_arm];
    //if (this->smart_resets) change_estimate = this->sr_change_estimates[pulled_arm];
    if (alarm_up) change_estimate = this->cdts_up[pulled_arm]->change_estimate;
    else change_estimate = this->cdts_down[pulled_arm]->change_estimate;

    /*if (this->smart_resets) {
      int history_amount = this->sr_collected_rewards[pulled_arm].size() - change_estimate;
      int t_start = change_estimate;
      if (history_amount > this->max_history) {
        t_start = this->sr_collected_rewards[pulled_arm].size() - this->max_history;
      }
      for (int i = t_start; i < this->sr_collected_rewards[pulled_arm].size(); i++) {
        this->sub_alg->receive_reward(this->sr_collected_rewards[pulled_arm][i], pulled_arm);
      }
    } else {*/
    int history_amount = this->collected_rewards[pulled_arm].size() - change_estimate;
    int t_start = change_estimate;
    if (history_amount > this->max_history) {
      t_start = this->collected_rewards[pulled_arm].size() - this->max_history;
    }
    for (int i = t_start; i < this->collected_rewards[pulled_arm].size(); i++) {
      this->sub_alg->receive_reward(this->collected_rewards[pulled_arm][i], pulled_arm);
    }
    //}
  }
  this->collected_rewards[pulled_arm].clear();
  //if (this->smart_resets) this->sr_collected_rewards[pulled_arm].clear();
  this->cdts_up[pulled_arm]->reset(0);
  this->cdts_down[pulled_arm]->reset(0);
  this->till_M[pulled_arm] = this->M;
}

void CD_Algorithm::receive_reward(double reward, int pulled_arm) {
  this->MABAlgorithm::receive_reward(reward, pulled_arm);
  this->sub_alg->receive_reward(reward, pulled_arm);
  this->collected_rewards[pulled_arm].push_back(reward);

  bool alarm_up = this->cdts_up[pulled_arm]->run(reward);
  bool alarm_down = this->cdts_down[pulled_arm]->run(reward);
  //if (alarm_up) this->num_alarms_up++;
  //if (alarm_down) this->num_alarms_down++;
  bool alarm = alarm_up || alarm_down;
  if (alarm_up) this->last_resets_up[pulled_arm] = true;
  else if (alarm_down) this->last_resets_up[pulled_arm] = false;

  /*
  bool sr_alarm_up = false;
  bool sr_alarm_down = false;
  if (this->smart_resets) {
    this->sr_collected_rewards[pulled_arm].push_back(reward);
    sr_alarm_up = this->sr_cdts_up[pulled_arm]->run(reward);
    sr_alarm_down = this->sr_cdts_down[pulled_arm]->run(reward);
    if (sr_alarm_up) this->sr_change_estimates[pulled_arm] = this->sr_cdts_up[pulled_arm]->change_estimate;
    else if (sr_alarm_down) this->sr_change_estimates[pulled_arm] = this->sr_cdts_down[pulled_arm]->change_estimate;
  }*/

  if (alarm) {
    if (!this->smart_resets) {
      this->reset_arm(pulled_arm);
    } else {
      // Directly check UCB1...
      UCB1* u = dynamic_cast<UCB1*>(this->get_sub_alg());
      int best_action = -1;
      if (u != NULL) {
        best_action = distance(u->means.begin(), max_element(u->means.begin(), u->means.end()));
      }

      this->to_be_reset.insert(pulled_arm);

      if (pulled_arm == best_action && alarm_down) {
        for (auto a : this->to_be_reset) {
          this->reset_arm(a);
        }
        this->to_be_reset.clear();
      } else if (pulled_arm != best_action && alarm_up) {
        vector<int> arms_to_reset;
        arms_to_reset.push_back(pulled_arm);
        if (this->to_be_reset.find(best_action) != this->to_be_reset.end()) {
          arms_to_reset.push_back(best_action);
        }
        for (auto a : arms_to_reset) {
          this->reset_arm(a);
          this->to_be_reset.erase(a);
        }
      } else {
        this->cdts_up[pulled_arm]->reset(1);
        this->cdts_down[pulled_arm]->reset(1);
      }
    }
  }

  // sr cdts
  /*
  if (this->smart_resets && (sr_alarm_up || sr_alarm_down)) {
    this->sr_cdts_up[pulled_arm]->reset(0);
    this->sr_cdts_down[pulled_arm]->reset(0);
    this->sr_collected_rewards[pulled_arm].clear();
  }*/

  this->timestep++;
}

ADAPT_EVE::ADAPT_EVE(string name, int num_of_arms, int meta_duration, string cdt_line, string sub_alg_line, boost::mt19937& rng, MAB* mab) : MetaAlgorithm(name, num_of_arms) {

  this->meta_duration = meta_duration;
  this->rng = &rng;

  for (int i = 0; i < this->num_of_arms; i++) {
    this->cdts.push_back(get_cdt(cdt_line));
  }

  this->sub_alg = get_algorithm(sub_alg_line, this->rng, mab);
  this->other_sub_alg = get_algorithm(sub_alg_line, this->rng, mab);

  vector<Distribution*> fake_distributions;
  fake_distributions.push_back(new FixedDistribution("", 0, rng));
  fake_distributions.push_back(new FixedDistribution("", 0, rng));
  MAB* fake_mab = new MAB(fake_distributions, mab->timesteps);
  this->meta_alg = get_algorithm(sub_alg_line, this->rng, fake_mab);

  this->reset(-1);
}

void ADAPT_EVE::reset(int action) {
	this->MABAlgorithm::reset(action);
  if (action == -1) {
    for (int i = 0; i < this->num_of_arms; i++) {
      this->cdts[i]->reset(0);
    }
  } else {
    this->cdts[action]->reset(0);
  }

  this->sub_alg->reset(action);
  this->other_sub_alg->reset(action);
  this->meta_alg->reset(action);

  this->is_meta = false;
  this->t_meta = 0;

  this->timestep = 0;
}

int ADAPT_EVE::choose_action() {
  int action = -1;
  if (!this->is_meta) {
    action = this->sub_alg->choose_action();
  } else {
    this->saved_meta_action = this->meta_alg->choose_action();
    if (this->saved_meta_action == 0) {
      action = this->sub_alg->choose_action();
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
    this->sub_alg->receive_reward(reward, pulled_arm);

    bool alarm = this->cdts[pulled_arm]->run(reward);
    if (alarm) {
      this->is_meta = true;
      this->t_meta = 0;

      this->other_sub_alg->reset(-1);
      this->meta_alg->reset(-1);
    }
  } else {
    t_meta++;

    this->meta_alg->receive_reward(reward, this->saved_meta_action);
    if (this->saved_meta_action == 0) {
      this->sub_alg->receive_reward(reward, pulled_arm);
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
        MABAlgorithm *temp = this->sub_alg;
        this->sub_alg = this->other_sub_alg;
        this->other_sub_alg = temp;

        //cout << "core pulls: " << core_pulls << ", other pulls: " << other_pulls << ", keeping new alg" << endl;
      } else {
        //cout << "core pulls: " << core_pulls << ", other pulls: " << other_pulls << ", keeping old alg" << endl;
      }

      this->is_meta = false;

      // Restart CDTs
      for (int i = 0; i < this->num_of_arms; i++) {
        this->cdts[i]->reset(0);
      }
    }
  }
}
/*
GLR::GLR(string name, int num_of_arms, int M, int max_history, string sub_alg_line, boost::mt19937& rng, MAB* mab) : MetaAlgorithm(name, num_of_arms) {
  this->sub_alg = get_algorithm(sub_alg_line, &rng, mab);
  this->max_history = max_history;
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
  this->cycle_reward(reward, pulled_arm);
  this->find_changepoints(pulled_arm);
  this->update_sub_alg(pulled_arm);
}

void GLR::cycle_reward(double new_reward, int arm_pulled) {
  this->rewards[arm_pulled].push_back(new_reward);
  if (this->rewards[arm_pulled].size() > this->max_history) {
    // Delete first element
    this->rewards[arm_pulled].erase(this->rewards[arm_pulled].begin(), this->rewards[arm_pulled].begin()+1);
  }
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
