#include "plotter.h"
#include <string>
#include <stdlib.h>

void Plotter::plot_experiments(int num_experiments) {
  string command = "python scripts/plot_results.py " + to_string(num_experiments) + " &";
	const char* c = command.c_str();
	system(c);
}
