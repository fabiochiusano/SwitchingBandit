#ifndef ADAPT_EVE_H
#define ADAPT_EVE_H

#include "mabalg.h"
#include "cdt.h"
#include <string>
#include <map>
#include <vector>

class ADAPT_EVE: public MABAlgorithm {
private:
  vector<CDT*> cdts;
  MABAlgorithm *core_sub_alg, *other_sub_alg, *meta_alg;
  MAB *meta_mab;

  double meta_duration;
  bool is_meta;
  double t_meta;

  int timestep;
public:
	ADAPT_EVE(string name, MAB& mab, int meta_duration, string cdt_line, string sub_alg_line, boost::mt19937& rng);
	ArmPull run(vector<double>& pulls, bool generate_new_pulls) override;
  void reset() override;
};

#endif
