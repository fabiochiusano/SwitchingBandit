#include "utils.h"

#include <stdlib.h>

using namespace std;

double random_unit() {
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}