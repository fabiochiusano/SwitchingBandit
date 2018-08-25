/**
 *  @file    plotter.h
 *  @author  Fabio Chiusano
 *  @date    08/06/2018
 *  @version 1.0
 */

#ifndef PLOTTER_H
#define PLOTTER_H

using namespace std;

/**
 * @brief Class adept at plotting the results of the experiments
 */
class Plotter {
public:
  /**
   * @brief plots the results of the experiments inside iìthe images folder
   */
  static void plot_experiment(int num_exp);
};

#endif
