/**
 *  @file    utils.h
 *  @author  Fabio Chiusano
 *  @date    08/06/2018
 *  @version 1.0
 *
 * @brief Contains utilities like random number generation, parsing of algorithm specifications
 * and file generation
 */

#ifndef UTILS_H
#define UTILS_H

#include "mabalg.h"
#include <string>
#include <map>
#include "cdt.h"

static int WRITE_EVERY = 50; // write to file every tot timestep (value used by statistic_manager and in plot_results.py)
static int NEG_INF = -10000000;
static int POS_INF = 10000000;

class CDT;

/**
 * @return a random double between zero and one
 */
double random_unit();

/**
 * @brief Create an algorithm from its specification
 *
 * @param  line        string, specification of the algorithm
 * @param  num_of_arms integer, number of arms the algorithm will work with
 * @param  rng         boost::mt19937*, random number generator used by the algorithm
 * @return             an instance of the algorithm devised in the specification
 */
MABAlgorithm* get_algorithm(string line, boost::mt19937* rng, MAB* mab);

/**
 * @brief Create a CDT from its specification
 *
 * @param  line        string, specification of the CDT
 * @return             an instance of the CDT devised in the specification
 */
CDT* get_cdt(string line);

/**
 * @brief Create an arm from its specification
 *
 * @param  line        string, specification of the arm distribution
 * @param  rng         boost::mt19937*, random number generator used by the algorithm
 * @return             an instance of the distribution devised in the specification
 */
Distribution* get_distribution(string line, boost::mt19937* rng);

/**
 * @param  data   vector of double, data to analyse
 * @param  moment integer, order of the moment to be computed
 * @return        the n-th moment computed from the data
 */
double get_moment(vector<double> data, int moment);

/**
 * @param  data   vector of double, data to analyse
 * @param  cumulant integer, order of the cumulant to be computed
 * @return        the n-th cumulant computed from the data
 */
double get_cumulant(vector<double> data, int cumulant);

/**
 * @brief Create a directory at the specified location
 *
 * @param name string, name of the directory
 */
void make_dir(string name);

/**
 * @brief Remove all the directories that contain data generated by the simulations
 */
void remove_dirs();

#endif
