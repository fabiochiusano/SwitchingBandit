#ifndef MABLOADER_H
#define MABLOADER_H

#include <vector>
#include "experiment.h"

using namespace std;

class ExperimentLoader {
private:
	static void parse_arm_line(MAB* mab, string line, boost::mt19937* rng);
	static void parse_alg_line(Experiment* experiment, MAB* mab, string line, boost::mt19937* rng);
public:
	static vector<Experiment*>* load_experiments();
};

#endif
