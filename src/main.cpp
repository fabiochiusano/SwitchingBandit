#include <boost/random.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <stdlib.h>

#include "mab.h"
#include "distribution.h"
#include "allalgorithms.h"
#include "experiment.h"

#include "experimentloader.h"
#include "plotter.h"

using namespace std;

int main(int argc, char *argv[]){
	vector<Experiment*>* experiments = ExperimentLoader::load_experiments();

	#pragma omp parallel for num_threads(4)
	for (int i = 0; i < experiments->size(); i++) {
		cout << "experiment " << i << " started" << endl;
		(*experiments)[i]->run();
	}

	Plotter::plot_experiments(experiments->size());

	return 0;
}
