#ifndef UTILS_H
#define UTILS_H

#include "mabalg.h"
#include <string>
#include <map>
#include "cdt.h"

static int WRITE_EVERY = 5; // write to file every 5 timestep (value used by statistic_manager and in plot_results.py)
static int NEG_INF = -100000;

class CDT;

double random_unit();

MABAlgorithm* get_algorithm(string line, int num_of_arms, boost::mt19937* rng);
CDT* get_cdt(string line);
Distribution* get_distribution(string line, boost::mt19937* rng);

double get_moment(vector<double> data, int moment);
double get_cumulant(vector<double> data, int cumulant);

void make_dir(string name);
void remove_dirs();

#endif
