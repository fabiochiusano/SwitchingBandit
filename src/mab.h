#ifndef MAB_H
#define MAB_H

#include <vector>

#include "mabalg.h"
#include "distribution.h"

using namespace std;

class MABAlgorithm;
class Distribution;

enum class MABType { Stochastic, Adversarial};

class MAB {
private:
	int id;

public:
	static int next_id;
	
	MABType* mabtype;
	vector<Distribution*> arms;

	MAB(MABType* mabtype);

	void addArm(Distribution* arm);

	vector<vector<double>> generate_pulls(int total_timesteps);

	int get_id();
};

#endif
