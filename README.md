# Switching Bandit
This repository contains the code for my thesis on switching bandits. The simulation code is written in C++ and uses _OpenMP_. The scripts that plot the results of the simulations are written in Python using _numpy_ and _matplotlib_.

## Algorithms
The algorithms implemented are:
- UCB
  - UCB1
  - UCBT
  - D-UCB
  - SW-UCB
- Thompson Sampling
  - Thompson Sampling for bernoulli arms
  - Thompson Sampling for gaussian arms
  - GlobalCTS
  - PerArmCTS
- EXP
  - EXP3
  - EXP3_S
  - REXP3
- Meta algorithms
  - Round: pulls each arm at least once before letting the subalgorithm decide
  - Algorithm with uniform exploration: sometimes chooses an arm at random, sometimes lets the subalgorithm decide
  - ADAPT_EVE
  - CD_algorithm: a subalgorithm coupled with a change detection test
  
## Distributions
You can choose arms with the following distributions:
- FixedDistribution: returns a constant value at each timestep
- FixedNonStationaryDistribution: returns a constant value at each timestep before the next change point
- BernoulliDistribution
- BernoulliNonStationaryDistribution
- NormalDistribution
- NormalNonStationaryDistribution
- SquareWaveDistribution: returns 0, 1, 0, 1, 0... It's useful since it's a simple example of an adversarial attack to UCB1.

## Change Detection Tests
The implemented CDTs are:
- Page-Hinkley
- Discounted Page-Hinkley
- Two-sided CUSUM
- ICI

## How to configure experiments
Instructions on how to configure experiments can be found in the README inside the _exp_config_ folder.
