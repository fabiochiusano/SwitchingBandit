#ifndef MABLOADER_H
#define MABLOADER_H

#include <vector>
#include "experiment.h"

using namespace std;

class ExperimentLoader {
public:
	static vector<Experiment*>* load_experiments();
};

#endif
