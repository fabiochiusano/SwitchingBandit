#include "utils.h"
#include "ucb.h"
#include "meta_algorithms.h"

Round_Algorithm::Round_Algorithm(string name, int num_of_arms, string sub_alg_line, boost::mt19937& rng) : MABAlgorithm(name, num_of_arms) {
  this->sub_alg = get_algorithm(sub_alg_line, num_of_arms, &rng);
  this->reset();
}

void Round_Algorithm::reset() {
	this->MABAlgorithm::reset();
  this->arms_pulled.assign(this->num_of_arms, false);
  this->sub_alg->reset();
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

void Algorithm_With_Uniform_Exploration::reset() {
	this->MABAlgorithm::reset();
  this->sub_alg->reset();
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



CD_Algorithm::CD_Algorithm(string name, int num_of_arms, string cdt_line, string sub_alg_line, bool use_history, boost::mt19937& rng) : MABAlgorithm(name, num_of_arms) {
  this->sub_alg = get_algorithm(sub_alg_line, num_of_arms, &rng);
  for (int i = 0; i < num_of_arms; i++) {
    this->cdts.push_back(get_cdt(cdt_line));
  }
  this->use_history = use_history;
  this->reset();
}

void CD_Algorithm::reset() {
  this->MABAlgorithm::reset();
  this->sub_alg->reset();
  for (int i = 0; i < this->num_of_arms; i++) {
    this->cdts[i]->reset();
  }
  this->collected_rewards.resize(this->num_of_arms);
  //this->chosen_arms.clear();
  //this->last_resets_global.assign(this->num_of_arms, -1);
}

int CD_Algorithm::choose_action() {
  return this->sub_alg->choose_action();
}

void CD_Algorithm::receive_reward(double reward, int pulled_arm) {
  this->MABAlgorithm::receive_reward(reward, pulled_arm);
  this->sub_alg->receive_reward(reward, pulled_arm);
  this->collected_rewards[pulled_arm].push_back(reward);

  //this->chosen_arm.push_back(pulled_arm);
  CDT_Result cdt_result = this->cdts[pulled_arm]->run(reward);
  if (cdt_result.alarm) {
    //cout << this->name << ": alarm raised at timestep " << timestep << " on arm " << armpull.arm_index << endl;

    this->sub_alg->reset();
    if (this->use_history) {
      for (int i = cdt_result.change_estimate; i < this->collected_rewards[pulled_arm].size(); i++) {
        this->sub_alg->receive_reward(this->collected_rewards[pulled_arm][i], pulled_arm);
      }
      this->collected_rewards[pulled_arm].clear();
    }

    this->cdts[pulled_arm]->reset();
  }
}

/*
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
*/

ADAPT_EVE::ADAPT_EVE(string name, int num_of_arms, int meta_duration, string cdt_line, string sub_alg_line, boost::mt19937& rng) : MABAlgorithm(name, num_of_arms) {

  this->meta_duration = meta_duration;
  //this->sub_alg_line = sub_alg_line;
  this->rng = &rng;

  for (int i = 0; i < this->num_of_arms; i++) {
    this->cdts.push_back(get_cdt(cdt_line));
  }

  this->core_sub_alg = get_algorithm(sub_alg_line, num_of_arms, this->rng);
  this->other_sub_alg = get_algorithm(sub_alg_line, num_of_arms, this->rng);

  /*
  MAB* meta_mab = new MAB(this->core_sub_alg->mab->mabtype);
  MAB::next_id--; // TODO: modify MAB::next_id--;
  MABDistribution *d_1 = new MABDistribution("Core", this->core_sub_alg);
  MABDistribution *d_2 = new MABDistribution("Other", this->other_sub_alg);
  meta_mab->addArm(d_1);
  meta_mab->addArm(d_2);
  */

  this->meta_alg = get_algorithm(sub_alg_line, 2, this->rng);

  this->reset();
}

void ADAPT_EVE::reset() {
	this->MABAlgorithm::reset();
  for (int i = 0; i < this->num_of_arms; i++) {
    this->cdts[i]->reset();
  }

  this->core_sub_alg->reset();
  this->other_sub_alg->reset();
  this->meta_alg->reset();

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

      this->other_sub_alg->reset();
      this->meta_alg->reset();
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

        /*
        // Swap algs also in meta_mab
        MAB* meta_mab = new MAB(this->core_sub_alg->mab->mabtype);
        MAB::next_id--; // TODO: modify MAB::next_id--;
        MABDistribution *d_1 = new MABDistribution("Core", this->core_sub_alg);
        MABDistribution *d_2 = new MABDistribution("Other", this->other_sub_alg);
        meta_mab->addArm(d_1);
        meta_mab->addArm(d_2);
        */
        //this->meta_alg = get_algorithm(this->sub_alg_line, 2, this->rng);

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
/*
ArmPull ADAPT_EVE::run(vector<vector<double>>& all_pulls, int timestep, bool generate_new_pulls) {
  vector<double> pulls = all_pulls[timestep];
  // pulls and highest mean are relative to the core mab (not the meta mab)
  ArmPull armpull(0,0);
  this->timestep++;

  if (!this->is_meta) {
    // If core algorithm is running
    armpull = this->core_sub_alg->run(all_pulls, timestep, generate_new_pulls);

    CDT_Result cdt_result = this->cdts[armpull.arm_index]->run(armpull.reward);
    if (cdt_result.alarm) {
      cout << this->name << ": alarm raised at timestep " << this->timestep << endl;

      this->is_meta = true;
      this->t_meta = 0;

      this->other_sub_alg->reset();
      this->meta_alg->reset();
    }
  } else {
    // If meta algorithm is running
    armpull = this->meta_alg->run(all_pulls, timestep, true);

    t_meta++;
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

        // Swap algs also in meta_mab
        MAB* meta_mab = new MAB(this->core_sub_alg->mab->mabtype);
        MAB::next_id--; // TODO: modify MAB::next_id--;
        MABDistribution *d_1 = new MABDistribution("Core", this->core_sub_alg);
        MABDistribution *d_2 = new MABDistribution("Other", this->other_sub_alg);
        meta_mab->addArm(d_1);
        meta_mab->addArm(d_2);
        this->meta_alg = get_algorithm(meta_mab, this->sub_alg_line, this->rng);

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

  return armpull;
}
*/
