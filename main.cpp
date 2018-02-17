#include <boost/random.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <stdlib.h>

#include "mab.h"
#include "distribution.h"
#include "mabalg.h"

using namespace std;

int main(int argc, char *argv[]){
	if (argc < 4) {
		cout << "Usage: " << endl;
		cout << "	timesteps" << endl;
		cout << "	first seed" << endl;
		cout << "	num experiments" << endl;
		return 0;
	}

	int TIME_STEPS = boost::lexical_cast<int>(argv[1]);
	int seed = boost::lexical_cast<int>(argv[2]);
	int num_experiments = boost::lexical_cast<int>(argv[3]);

	boost::mt19937 rng(seed);
	srand(seed);

	MABType mabtype = MABType::Stochastic;
	MAB mab(mabtype);

	mab.addArm(new NormalDistribution("G1", 2.0f, 1.5f, rng));
	mab.addArm(new NormalDistribution("G2", 1.0f, 0.5f, rng));
	mab.addArm(new NormalDistribution("G3", 5.5f, 2.0f, rng));
	mab.addArm(new UniformDistribution("G4", 2.5f, rng));

	mab.plot_distributions();
	mab.plot_means_in_time(TIME_STEPS);

	vector<MABAlgorithm*> algs;

	algs.push_back(new UCB1("UCB1", mab));
	algs.push_back(new UCBT("UCBT", mab));
	algs.push_back(new GLIE("GLIE=1", mab, 1.0));
	//algs.push_back(new E_Greedy("E_greedy_e=0.2", mab, 0.2));
	//algs.push_back(new E_Greedy("E_greedy_e=0.1", mab, 0.1));
	algs.push_back(new ThompsonSamplingGaussian("TS_G", mab, rng));
	//algs.push_back(new ThompsonSamplingBernoulli("TS_B", mab, rng));

	for (int cur_experiment = 1; cur_experiment <= num_experiments; cur_experiment++) {
		for (int timestep = 0; timestep < TIME_STEPS; timestep++) {
			mab.run(algs, timestep, TIME_STEPS);
		}

		if (cur_experiment == 1) {
			mab.write_regrets(algs, false);
		} else {
			mab.write_regrets(algs, true);
		}

		mab.reset(algs);
	}

	mab.plot_regrets();

	return 0;
}
