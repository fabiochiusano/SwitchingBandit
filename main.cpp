#include <boost/random.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <stdlib.h>

#include "mab.h"
#include "distribution.h"
#include "mabalg.h"
#include "experiment.h"

#include "experimentloader.h"
#include "plotter.h"

using namespace std;

int main(int argc, char *argv[]){
	vector<Experiment*>* experiments = ExperimentLoader::load_experiments();

	for (auto experiment : (*experiments)) {
		experiment->run();
	}

	Plotter::plot_experiments(experiments->size());

	return 0;
}
