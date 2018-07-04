#include "adapt_eve.h"
#include "utils.h"

// TODO: add destructors in order to free dynamic memory (everywhere)
/*
ADAPT_EVE::ADAPT_EVE(string name, MAB& mab, int meta_duration, string cdt_line, string sub_alg_line, boost::mt19937& rng) : MABAlgorithm(name, mab) {

  this->meta_duration = meta_duration;
  this->sub_alg_line = sub_alg_line;
  this->rng = &rng;

  for (int i = 0; i < this->num_of_arms; i++) {
    this->cdts.push_back(get_cdt(cdt_line));
  }

  this->core_sub_alg = get_algorithm(&mab, sub_alg_line, this->rng);
  this->other_sub_alg = get_algorithm(&mab, sub_alg_line, this->rng);

  MAB* meta_mab = new MAB(this->core_sub_alg->mab->mabtype);
  MAB::next_id--; // TODO: modify MAB::next_id--;
  MABDistribution *d_1 = new MABDistribution("Core", this->core_sub_alg);
  MABDistribution *d_2 = new MABDistribution("Other", this->other_sub_alg);
  meta_mab->addArm(d_1);
  meta_mab->addArm(d_2);

  this->meta_alg = get_algorithm(meta_mab, this->sub_alg_line, this->rng);

  this->reset();
}

void ADAPT_EVE::reset() {
	mabalg_reset();
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
