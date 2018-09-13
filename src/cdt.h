/**
 *  @file    cdt.h
 *  @author  Fabio Chiusano
 *  @date    08/06/2018
 *  @version 1.0
 *
 * @brief contains some CDT procedures, such as the omnibus test Page-Hinkley with
 * a discounted variant, the ICI procedure and the two-sided CUSUM algorithm.
 */

#ifndef CDT_H
#define CDT_H

#include <vector>
#include "utils.h"

using namespace std;

/**
 * @brief Base class of all the CDT procedures.
 */
class CDT {
public:
	int change_estimate = 0;
public:
	virtual bool run(double reward) = 0;
  virtual void reset(int mode) = 0; // if 0 -> reset completely, if 1 -> reset only the walk
};

/**
 * @brief CDT with the omnibus test Page-Hinkley.
 */
class CDT_PH: public CDT {
private:
  double gamma, lambda, PH, min_PH;
  double mean_reward;
  int num_rewards;
public:
	/**
	 * @brief Constructor of CDT_PH, i.e. a CDT with the omnibus Page-Hinkley test.
	 *
	 * @param gamma  double, minimum expected mean variation
	 * @param lambda double, threshold for the PH statistic
	 */
  CDT_PH(double gamma, double lambda);

	/**
	 * @brief run the CDT_PH algorithm
	 *
	 * @param  reward double, the new datum that must be analysed by the CDT
	 * @return a CDT_Result that contains the alarm and the estimated timestep change
	 */
  bool run(double reward) override;

	/**
	 * @brief reset the CDT_PH algorithm
	 */
  void reset(int mode) override;
};

/**
 * @brief CDT with the discounted omnibus test Page-Hinkley.
 */
class CDT_PH_RHO: public CDT {
private:
  double gamma, lambda, rho, PH;
  double mean_reward;
  int num_rewards;
public:
	/**
	 * @brief Constructor of CDT_PH_RHO, i.e. a CDT with the discounted omnibus Page-Hinkley test.
	 *
	 * @param gamma  double, minimum expected mean variation
	 * @param lambda double, threshold for the PH statistic
	 * @param rho    double, discount factor
	 */
  CDT_PH_RHO(double gamma, double lambda, double rho);

	/**
	 * @brief run the CDT_PH_RHO algorithm
	 *
	 * @param  reward double, the new datum that must be analysed by the CDT
	 * @return a CDT_Result that contains the alarm and the estimated timestep change (this
	 * 	second part is not implemented!)
	 */
  bool run(double reward) override;

	/**
	 * @brief reset the CDT_PH_RHO algorithm
	 */
  void reset(int mode) override;
};

/**
 * @brief CDT with the CUSUM algorithm.
 */
class CUSUM: public CDT {
private:
	int M; // number of samples required to have a reliable mean
	double epsilon; // something to remove from each received sample: it's the expected mean variation, assuming gaussian samples
	double threshold; // threshold for the alarm
	double mean_over_M, g;
	double cumul, min_cumul;
	bool gaussian, increase;
	int num_rewards;
public:
	//static int N_POS;
	//static int N_NEG;
	/**
	 * @brief Constructor of Two_Sided_CUSUM, i.e. the CUSUM algorithm.
	 *
	 * @param M         integer, CUSUM initialization steps used for computing the mean of the current sample distribution
	 * @param epsilon   double, minimum expected mean variation
	 * @param threshold double, threshold for the CUSUM walk
	 * @param gaussian  boolean, whether to use the gaussian update rule or the bernoulli update rule
	 * @param increse   boolean, whether to monitor increases or decreases in the mean
	 */
	CUSUM(int M, double epsilon, double threshold, bool gaussian, bool increase);

	/**
	 * @brief run the Two_Sided_CUSUM algorithm
	 *
	 * @param  reward double, the new datum that must be analysed by the CDT
	 * @return a CDT_Result that contains the alarm and the estimated timestep change
	 */

	bool run(double reward) override;

	/**
	 * @brief reset the Two_Sided_CUSUM algorithm
	 */
	void reset(int mode) override;
};
/*
class Two_Sided_CUSUM: public CDT {
private:
	int M; // number of samples required to have a reliable mean
	double epsilon; // something to remove from each received sample: it's the expected mean variation, assuming gaussian samples
	double threshold; // threshold for the alarm
	double mean_over_M, g_minus, g_plus;
	double cum_plus, min_cum_plus, cum_minus, min_cum_minus;
	bool gaussian;
	int t_estimate_plus, t_estimate_minus;
	int num_rewards;
public:

	Two_Sided_CUSUM(int M, double epsilon, double threshold, bool gaussian);



	CDT_Result run(double reward) override;

	void reset(int mode) override;
};
*/

/*
class ICI: public CDT {
private:
	int S0; // number of intervals for initialization
	int nu; // length of intervals
	double gamma; // confidence parameter

	bool in_init;
	int period;
	double h0;
	vector<double> reward_buffer;
	double M_inf, M_sup, V_inf, V_sup;
	double M_sigma_init, V_sigma_init;
	double M_mu, M_sigma, V_mu, V_sigma;
public:

	ICI(int S0, int nu, double gamma);


	CDT_Result run(double reward) override;


	void reset(int mode) override;
};
*/

#endif
