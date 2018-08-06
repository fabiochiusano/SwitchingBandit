#include "cdt.h"
#include <algorithm>


CDT_Result::CDT_Result(bool alarm, int change_estimate) {
  this->alarm = alarm;
  this->change_estimate = change_estimate;
}

CDT_PH::CDT_PH(double gamma, double lambda) {
  this->gamma = gamma;
  this->lambda = lambda;
  this->reset();
}

Two_Sided_CUSUM::Two_Sided_CUSUM(int M, double epsilon, double threshold, bool gaussian) {
  this->M = M;
  this->epsilon = epsilon;
  this->threshold = threshold;
  this->gaussian = gaussian;
  this->reset();
}

CDT_PH_RHO::CDT_PH_RHO(double gamma, double lambda, double rho) {
  this->gamma = gamma;
  this->lambda = lambda;
  this->rho = rho;
  this->reset();
}

ICI::ICI(int S0, int nu, double gamma) {
  this->S0 = S0;
  this->nu = nu;
  this->gamma = gamma;
  this->reset();
}

void CDT_PH::reset() {
  this->PH = 0;
  this->mean_reward = 0;
  this->num_rewards = 0;
}

void CDT_PH_RHO::reset() {
  this->PH = 0;
  this->mean_reward = 0;
  this->num_rewards = 0;
}

void Two_Sided_CUSUM::reset() {
  this->mean_over_M = 0;
  this->g_minus = 0;
  this->g_plus = 0;
  this->num_rewards = 0;
  this->cum_plus = 0;
  this->min_cum_plus = 0;
  this->cum_minus = 0;
  this->min_cum_minus = 0;
  this->t_estimate_plus = 0;
  this->t_estimate_minus = 0;
}

void ICI::reset() {
  this->in_init = true;
  this->period = this->S0;
  this->reward_buffer.clear();
}

CDT_Result CDT_PH::run(double reward) {
  // Update reward mean
  this->num_rewards++;
  this->mean_reward = (this->mean_reward * (this->num_rewards - 1) + reward) / this->num_rewards;

  // Update PH statistic
  this->PH = max(this->PH - reward + this->mean_reward - this->gamma, 0.);

  // Return alarm
  return CDT_Result(this->PH > this->lambda, this->num_rewards); // the estimation of the timestep of the change is not implemented
}

CDT_Result CDT_PH_RHO::run(double reward) {
  // Update reward mean
  this->num_rewards++;
  this->mean_reward = (this->mean_reward * (this->num_rewards - 1) + reward) / this->num_rewards;

  // Update PH statistic
  this->PH = max((this->rho * this->PH) - reward + this->mean_reward - this->gamma, 0.);

  // Return alarm
  return CDT_Result(this->PH > this->lambda, this->num_rewards);
}

CDT_Result Two_Sided_CUSUM::run(double reward) {
  // Update reward mean
  this->num_rewards++;
  if (this->num_rewards <= this->M) {
    this->mean_over_M += reward;
  }
  if (this->num_rewards == this->M) {
    this->mean_over_M /= this->M;
  }

  if (this->num_rewards <= this->M) {
    return CDT_Result(false, 0);
  } else {
    double s_plus = 0;
    double s_minus = 0;
    if (this->gaussian) {
      s_plus = this->epsilon * (reward - this->mean_over_M - this->epsilon/2);
      s_minus = this->epsilon * (this->mean_over_M - reward  - this->epsilon/2);
    } else {
      if (reward > 0.5) { // i.e. reward = 1
        s_plus = log(1 + this->epsilon/this->mean_over_M);
        s_minus = log(1 - this->epsilon/this->mean_over_M);
      } else { // i.e. reward = 0
        s_plus = log(1 - this->epsilon/(1 - this->mean_over_M));
        s_minus = log(1 + this->epsilon/(1 - this->mean_over_M));
      }
    }

    this->g_plus = max(0., this->g_plus + s_plus);
    this->cum_plus = this->cum_plus + s_plus;
    if (this->cum_plus <= this->min_cum_plus) {
      this->min_cum_plus = this->cum_plus;
      this->t_estimate_plus = this->num_rewards;
    }

    this->g_minus = max(0., this->g_minus + s_minus);
    this->cum_minus = this->cum_minus + s_minus;
    if (this->cum_minus <= this->min_cum_minus) {
      this->min_cum_minus = this->cum_minus;
      this->t_estimate_minus = this->num_rewards;
    }

    if (this->g_plus > this->threshold) {
      return CDT_Result(true, this->t_estimate_plus);
    }
    else if (this->g_minus > this->threshold) {
      return CDT_Result(true, this->t_estimate_minus);
    }

    return CDT_Result(false, 0);
  }
}

CDT_Result ICI::run(double reward) {
  bool result = false;

  this->reward_buffer.push_back(reward);

  // If initialization/training phase
  if (this->in_init) {
    // If end of initialization phase
    if (this->reward_buffer.size() == this->S0 * this->nu) {
      // M
      vector<double> ms;
      for (int s = 1; s <= this->S0; s++) {
        double tot = 0;
        for (int t = (s-1)*this->nu; t < s * this->nu; t++) {
          tot += this->reward_buffer[t];
        }
        ms.push_back(tot/this->nu);
      }

      // M_mu and M_sigma
      this->M_mu = accumulate(ms.begin(), ms.end(), 0.) / this->S0;
      double tot = 0;
      for (int s = 1; s <= this->S0; s++) {
        tot += pow((ms[s-1] - this->M_mu), 2);
      }
      tot = sqrt(tot/(this->S0 - 1));
      this->M_sigma_init = tot;
      this->M_sigma = tot/sqrt(this->S0);

      // M_inf and M_sup
      this->M_inf = this->M_mu - this->gamma * this->M_sigma;
      this->M_sup = this->M_mu + this->gamma * this->M_sigma;

      // V
      double k1, k2, k3;
      k1 = get_cumulant(this->reward_buffer, 1);
      k2 = get_cumulant(this->reward_buffer, 2);
      k3 = get_cumulant(this->reward_buffer, 3);
      this->h0 = 1 - (k1 * k3) / (3 * pow(k2, 2));
      vector<double> vs;
      for (int s = 1; s <= this->S0; s++) {
        double tot = 0;
        for (int t = (s-1)*this->nu; t < s * this->nu; t++) {
          tot += pow((this->reward_buffer[t] - ms[s-1]), 2);
        }
        vs.push_back(pow((tot / (this->nu - 1)), this->h0));
      }

      // V_mu and V_sigma
      this->V_mu = accumulate(vs.begin(), vs.end(), 0.) / this->S0;
      tot = 0;
      for (int s = 1; s <= this->S0; s++) {
        tot += pow((vs[s-1] - this->V_mu), 2);
      }
      tot = sqrt(tot/(this->S0 - 1));
      this->V_sigma_init = tot;
      this->V_sigma = tot/sqrt(this->S0);

      // V_inf and V_sup
      this->V_inf = this->V_mu - this->gamma * this->V_sigma;
      this->V_sup = this->V_mu + this->gamma * this->V_sigma;

      // Initialization ended
      this->in_init = false;
      this->reward_buffer.clear();
    }
  }
  // If detection phase
  else {
    // if enough observations (the only place where "result" can become true)
    if (this->reward_buffer.size() == this->nu) {
      // New period
      this->period++;

      // Compute M and V of the new batch
      double m = accumulate(this->reward_buffer.begin(), this->reward_buffer.end(), 0.) / this->nu;
      double v = 0;
      for (int t = 0; t < this->nu; t++) {
        v += pow((this->reward_buffer[t] - m), 2);
      }
      v = pow(v / (this->nu - 1), this->h0);

      // M_mu and M_sigma, V_mu and V_sigma
      this->M_mu = ((this->period - 1) * this->M_mu + m) / this->period;
      this->M_sigma = this->M_sigma_init / sqrt(this->period);
      this->V_mu = ((this->period - 1) * this->V_mu + v) / this->period;
      this->V_sigma = this->V_sigma_init / sqrt(this->period);

      // New confidence bounds
      double M_inf_new = this->M_mu - this->gamma * this->M_sigma;
      double M_sup_new = this->M_mu + this->gamma * this->M_sigma;
      double V_inf_new = this->V_mu - this->gamma * this->V_sigma;
      double V_sup_new = this->V_mu + this->gamma * this->V_sigma;

      // Intersect intervals
      this->M_inf = max(this->M_inf, M_inf_new);
      this->M_sup = min(this->M_sup, M_sup_new);
      this->V_inf = max(this->V_inf, V_inf_new);
      this->V_sup = min(this->V_sup, V_sup_new);

      // Is the intersection empty?
      if (this->M_inf > this->M_sup) {// || this->V_inf > this->V_sup) {
        result = true;
      }

      // Clear buffer
      this->reward_buffer.clear();
    }
  }

  return CDT_Result(result, this->reward_buffer.size());
}
