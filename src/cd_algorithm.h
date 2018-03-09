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
  int timestep;
public:
  CD_Algorithm(string name, MAB& mab, string cdt_line, string sub_alg_line, boost::mt19937& rng);
	ArmPull run(vector<double>& pulls, bool generate_new_pulls) override;
  void reset() override;
};

#endif
