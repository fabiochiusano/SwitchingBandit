#include "plotter.h"
#include <string>
#include <stdlib.h>

void Plotter::plot_experiments() {
  string command = "python scripts/plot_results.py";
	const char* c = command.c_str();
	system(c);
}
