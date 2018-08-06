/**
 *  @file    experimentloader.h
 *  @author  Fabio Chiusano
 *  @date    08/06/2018
 *  @version 1.0
 */

#ifndef MABLOADER_H
#define MABLOADER_H

#include <vector>
#include "experiment.h"

using namespace std;

/**
 * @brief class whose main goal is to offer a static method which returns a vector of experiments
 * loaded from a configuration file.
 */
class ExperimentLoader {
public:
	/**
	 * @return a vector of experiments loaded from a configuration file.
	 */
	static vector<Experiment*>* load_experiments();
};

#endif
