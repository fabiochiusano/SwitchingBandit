#include "ucb.h"
#include "utils.h"


UCB::UCB(string name, int num_of_arms) : MABAlgorithm(name, num_of_arms) {}

UCB1::UCB1(string name, int num_of_arms) : UCB(name, num_of_arms) {
	this->reset(-1);
}

UCBT::UCBT(string name, int num_of_arms) : UCB(name, num_of_arms) {
	this->reset(-1);
}

D_UCB::D_UCB(string name, int num_of_arms, double gamma, double B, double epsilon) : UCB(name, num_of_arms) {
	this->reset(-1);
	this->gamma = gamma;
	this->B = B;
	this->epsilon = epsilon;
}

SW_UCB::SW_UCB(string name, int num_of_arms, int tau, double B, double epsilon) : UCB(name, num_of_arms) {
	this->reset(-1);
	this->tau = tau;
	this->B = B;
	this->epsilon = epsilon;
}


void UCB1::reset(int action) {
	this->MABAlgorithm::reset(action);
	if (action == -1) {
		this->means.assign(this->num_of_arms, 0.);
	}
	else {
		this->means[action] = 0;
	}
}

void UCBT::reset(int action) {
	this->MABAlgorithm::reset(action);
	if (action == -1) {
		this->means.assign(this->num_of_arms, 0.);
		this->variances.assign(this->num_of_arms, 0.);
	}
	else {
		this->means[action] = 0;
		this->variances[action] = 0;
	}
}

void D_UCB::reset(int action) {
	this->MABAlgorithm::reset(action);
	if (action == -1) {
		this->means.assign(this->num_of_arms, 0.);
		this->ns.assign(this->num_of_arms, 0.);
	}
	else {
		this->means[action] = 0;
		this->ns[action] = 0;
	}
}

void SW_UCB::reset(int action) {
	this->MABAlgorithm::reset(action);
	if (action == -1) {
		this->windowed_arm_pulls.clear();
		this->windowed_arm_pulls_values.clear();
		this->windowed_arm_pulls.resize(this->num_of_arms);
		this->windowed_arm_pulls_values.resize(this->num_of_arms);
	}
	else {
		this->windowed_arm_pulls[action].clear();
		this->windowed_arm_pulls_values[action].clear();
	}
}

int UCB1::choose_action() {
	int tot_pulls = accumulate(this->num_of_pulls.begin(), this->num_of_pulls.end(), 0.);

	// pull arm that maximizes Q+B
	double bestQB = NEG_INF;
	int best_arm = -1;
	for (int i = 0; i < this->num_of_arms; i++) {
		double Q = this->means[i];
		double B = sqrt((2*log(tot_pulls + 1)) / max(this->num_of_pulls[i],1));
		double QB = Q + B;
		if (QB > bestQB) {
			bestQB = QB;
			best_arm = i;
		}
	}
	return best_arm;
}

void UCB1::receive_reward(double reward, int pulled_arm) {
	this->MABAlgorithm::receive_reward(reward, pulled_arm);
	this->means[pulled_arm] = (this->means[pulled_arm] * (this->num_of_pulls[pulled_arm] - 1) + reward) / this->num_of_pulls[pulled_arm];
}

int UCBT::choose_action() {
	int tot_pulls = accumulate(this->num_of_pulls.begin(), this->num_of_pulls.end(), 0.);

	// Pull arm that maximizes Q+B
	double bestQB = NEG_INF;
	int best_arm = -1;
	for (int i = 0; i < this->num_of_arms; i++) {
		double Q = this->means[i];
		double variance = this->variances[i];
		double B = sqrt((2*log(tot_pulls + 1)*min(0.25, variance)) / max(this->num_of_pulls[i],1));
		double QB = Q + B;
		if (QB > bestQB) {
			bestQB = QB;
			best_arm = i;
		}
	}

	return best_arm;
}

void UCBT::receive_reward(double reward, int pulled_arm) {
	this->MABAlgorithm::receive_reward(reward, pulled_arm);
	double old_mean = this->means[pulled_arm];
	int n = this->num_of_pulls[pulled_arm];
	this->means[pulled_arm] = (old_mean * (n - 1) + reward) / n;
	this->variances[pulled_arm] = ((this->variances[pulled_arm] + pow(old_mean, 2))*(n - 1) + pow(reward, 2)) / n - pow(this->means[pulled_arm], 2);
}

int D_UCB::choose_action() {
	// Pull arm that maximizes Q+B
	double bestQB = NEG_INF;
	int best_arm = -1;
	double ns_sum = accumulate(this->ns.begin(), this->ns.end(), 0);
	for (int i = 0; i < this->num_of_arms; i++) {
		double Q = this->means[i];
		double B = (2 * this->B) * sqrt((this->epsilon * log(max(ns_sum, 1.))) / max(this->ns[i], 0.000001));
		double QB = Q + B;
		if (QB > bestQB) {
			bestQB = QB;
			best_arm = i;
		}
	}

	return best_arm;
}

void D_UCB::receive_reward(double reward, int pulled_arm) {
	this->MABAlgorithm::receive_reward(reward, pulled_arm);

	// Update this->ns (it's like this->num_of_pulls, but discounted with this->gamma)
	vector<double> old_ns;
	old_ns.assign(this->num_of_arms, 0.);
	copy(this->ns.begin(), this->ns.end(), old_ns.begin());
	for (int i = 0; i < this->num_of_arms; i++) {
		this->ns[i] *= this->gamma;
	}
	this->ns[pulled_arm] += 1.0;

	// Update this->means (it's like a normal mean, but discounted with this->gamma)
	for (int i = 0; i < this->num_of_arms; i++) {
		this->means[i] *= old_ns[i];
	}
	this->means[pulled_arm] += reward;
	for (int i = 0; i < this->num_of_arms; i++) {
		if (this->ns[i] > 0) { // Because we don't want to divide by zero...
			this->means[i] *= this->gamma / this->ns[i];
		} else {
			this->means[i] = POS_INF;
		}
	}
}

int SW_UCB::choose_action() {
	int tot_pulls = accumulate(this->num_of_pulls.begin(), this->num_of_pulls.end(), 0.);

	// Pull arm that maximizes Q+B
	double bestQB = NEG_INF;
	int best_arm = -1;
	for (int i = 0; i < this->num_of_arms; i++) {
		int windowed_N = accumulate(this->windowed_arm_pulls[i].begin(), this->windowed_arm_pulls[i].end(), 0);
		double windowed_reward = accumulate(this->windowed_arm_pulls_values[i].begin(), this->windowed_arm_pulls_values[i].end(), 0.);
		double Q = windowed_reward / max(windowed_N, 1);
		double B = this->B * sqrt((this->epsilon * log(min(this->tau, tot_pulls + 1))) / max(windowed_N, 1));
		double QB = Q + B;

		if (QB > bestQB) {
			bestQB = QB;
			best_arm = i;
		}
	}

	return best_arm;
}

void SW_UCB::receive_reward(double reward, int pulled_arm) {
	this->MABAlgorithm::receive_reward(reward, pulled_arm);

	// Move the window
	for (int i = 0; i < this->num_of_arms; i++) {
		if (i == pulled_arm) {
			this->windowed_arm_pulls[i].push_back(1);
			this->windowed_arm_pulls_values[i].push_back(reward);
		} else {
			this->windowed_arm_pulls[i].push_back(0);
			this->windowed_arm_pulls_values[i].push_back(0.);
		}

		if (this->windowed_arm_pulls[i].size() > this->tau)
			this->windowed_arm_pulls[i].erase(this->windowed_arm_pulls[i].begin());
		if (this->windowed_arm_pulls_values[i].size() > this->tau)
			this->windowed_arm_pulls_values[i].erase(this->windowed_arm_pulls_values[i].begin());
	}
}
