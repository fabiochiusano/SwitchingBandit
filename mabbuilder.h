#ifndef MABBUILDER_H
#define MABBUILDER_H

#include <vector>
#include "distribution.h"

using namespace std;

class MABBuilder {
public:
	vector<Distribution*> arms;

	void addArm(Distribution* distr);
	void getMAB();
};

#endif