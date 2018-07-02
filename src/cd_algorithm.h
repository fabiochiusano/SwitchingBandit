#ifndef CD_ALGORITHM_H
#define CD_ALGORITHM_H

#include "mabalg.h"
#include "cdt.h"
#include <string>
#include <vector>

class CD_Algorithm: public MABAlgorithm {
private:
  vector<CDT*> cdts;
  MABAlgorithm* alg;
  bool use_history;
  double alpha; // In [0,1]. High alpha -> high exploration.
  vector<int> chosen_arms;
  vector<int> last_resets_global;
  void update_history_UCB(vector<vector<double>>& all_pulls, int timestep, int arm_reset, CDT_Result cdt_result);
public:
  CD_Algorithm(string name, MAB& mab, string cdt_line, string sub_alg_line, bool use_history, double alpha, boost::mt19937& rng);
	ArmPull run(vector<vector<double>>& all_pulls, int timestep, bool generate_new_pulls) override;
  void reset() override;
};

#endif
