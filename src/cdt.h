#ifndef CDT_H
#define CDT_H

#include <vector>
#include "utils.h"

using namespace std;

class CDT_Result {
public:
	bool alarm;
	int change_estimate;
	CDT_Result(bool alarm, int change_estimate);
};

class CDT {
public:
	virtual CDT_Result run(double reward) = 0;
  virtual void reset() = 0;
};

class CDT_PH: public CDT {
private:
  double gamma, lambda, PH;
  double mean_reward;
  int num_rewards;
public:
  CDT_PH(double gamma, double lambda);
  CDT_Result run(double reward) override;
  void reset() override;
};

class CDT_PH_RHO: public CDT {
private:
  double gamma, lambda, rho, PH;
  double mean_reward;
  int num_rewards;
public:
  CDT_PH_RHO(double gamma, double lambda, double rho);
  CDT_Result run(double reward) override;
  void reset() override;
};

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
	void reset() override;
};

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
	void reset() override;
};

#endif
