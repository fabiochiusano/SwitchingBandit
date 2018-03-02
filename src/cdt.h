#ifndef CDT_H
#define CDT_H

using namespace std;

class CDT {
public:
	virtual bool run(double reward) = 0;
  virtual void reset() = 0;
};

class CDT_PH: public CDT {
private:
  double gamma, lambda, PH;
  double mean_reward;
  int num_rewards;
public:
  CDT_PH(double gamma, double lambda);
  bool run(double reward) override;
  void reset() override;
};

class CDT_PH_RHO: public CDT {
private:
  double gamma, lambda, rho, PH;
  double mean_reward;
  int num_rewards;
public:
  CDT_PH_RHO(double gamma, double lambda, double rho);
  bool run(double reward) override;
  void reset() override;
};

#endif
