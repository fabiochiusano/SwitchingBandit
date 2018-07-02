#ifndef UTILS_H
#define UTILS_H

#include "mabalg.h"
#include <string>
#include <map>
#include "cdt.h"

class CDT;

double random_unit();

MABAlgorithm* get_algorithm(MAB* mab, string line, boost::mt19937* rng);
CDT* get_cdt(string line);
Distribution* get_distribution(string line, boost::mt19937* rng);

double get_moment(vector<double> data, int moment);
double get_cumulant(vector<double> data, int cumulant);

#endif
