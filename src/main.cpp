/**
 *  @file    main.cpp
 *  @author  Fabio Chiusano
 *  @date    08/06/2018
 *  @version 1.0
 *
 *  @brief Entry of the program. Load the experiments from temp/experiments_config.txt and run them.
 *
 */

#include <boost/random.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <stdlib.h>
#include <omp.h>

#include "mab.h"
#include "distribution.h"
#include "allalgorithms.h"
#include "experiment.h"
#include "input_parser.h"

#include "experimentloader.h"
#include "plotter.h"
#include "utils.h"

using namespace std;

/**
 * Entry of the program. Load the experiments from temp/experiments_config.txt and run them.
 * Command options:
 * 	-p: for parallel execution with 4 threads (1 thread otherwise)
 *
 * @param  argc [...]
 * @param  argv [...]
 * @return      [...]
 */
int main(int argc, char *argv[]){
	InputParser ip(argc, argv);

	if (ip.cmdOptionExists("-h")) {
		cout << "Load experiments from temp/experiments_config.txt" << endl;
		cout << "Command options:" << endl;
		cout << "\t-p for parallel execution with 4 threads (1 thread otherwise)" << endl;
		return 0;
	}

	if (ip.cmdOptionExists("-p")) {
		cout << "Running with 4 threads" << endl;
		omp_set_num_threads(4);
	} else {
		cout << "Running with 1 thread" << endl;
		omp_set_num_threads(1);
	}

	remove_dirs();
	vector<Experiment*>* experiments = ExperimentLoader::load_experiments();

	#pragma omp parallel for
	for (int i = 0; i < experiments->size(); i++) {
		cout << "experiment " << i << " started" << endl;
		(*experiments)[i]->run();
		Plotter::plot_experiment(i);
	}

	return 0;
}
