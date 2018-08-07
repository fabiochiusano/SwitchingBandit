# HOW TO CONFIGURE EXPERIMENTS 
In order to configure experiments, you need to fill the _experiments_config.txt_ file in this folder.

## EXAMPLE:
```1 30 432145
first 0 2 2 10000
BernoulliNonStationary 2 0.2 5000 0.8 10000
BernoulliNonStationary 1 0.5 10000
ucb1_round round 2 ucb1 ucb1
ucbt_round round 2 ucbt ucbt
```

## EXAMPLE DESCRIPTION:
```1 30 432145 ---------- number_of_experiments simulations_per_experiment seed
first 0 2 2 10000 ---------- experiment_name regret_type num_of_arms num_of_algorithms time_horizon
BernoulliNonStationary 2 0.2 5000 0.8 10000 ---------- distribution_specification
BernoulliNonStationary 1 0.5 10000 ---------- distribution_specification
ucb1_round round 2 ucb1 ucb1 ---------- algorithm_specification
ucbt_round round 2 ucbt ucbt ---------- algorithm_specification
```

After the first line, there are _number_of_experiments_ blocks, each one corresponding to an _experiment_specification_.
An _experiment_specification_ is composed of:
- _experiment_info_
- _distributions_
- _algortihms_

### _EXPERIMENT_INFO_
_experiment_info_ is a line composed by: experiment_name regret_type num_of_arms num_of_algorithms time_horizon. `regret_type == 0` means stochastic regret, `regret_type == 1` means adversarial regret.
_experiment_info_ is a line composed by: ```experiment_name regret_type num_of_arms num_of_algorithms time_horizon``` 

### _DISTRIBUTIONS_
_distributions_ is a list of _distribution_specification_. Examples of _distribution_specification_ are:
- `BernoulliNonStationary 2 0.2 5000 0.8 10000 ---------- distribution_id number_of_change_points (mean till_timestep)^(number_of_change_points)`
- `Bernoulli 0.4 ---------- distribution_id mean`
- `NormalNonStationary 2 0.5 0.3 6000 0.8 0.5 10000 ---------- distribution_id number_of_change_points (mean stddev till_timestep)^(number_of_change_points)`
- `Normal 0.5 0.2 ---------- distribution_id mean variance`
Other distributions specifications can be inferred reading the parser implementation, which is the _get_distribution_ function in _utils.cpp_.

"algorithms" is a list of "algorithm_specification". Examples of "algorithm_specification" are:
- ucb1 ucb1 ---------- alg_name alg_id
- ucbt ucbt ---------- alg_name alg_id
- ts_b ts_b ---------- alg_name alg_id
- exp3 exp3 0.1 3 ---------- alg_name alg_id beta nu
- ucb1_round round 2 ucb1 ucb1 ---------- alg_name alg_id length_of_subalg_specification subalg_specification
Since "round" is a meta-algorithm that requires a sub-algorithm, its specification needs the sub-algorithm specification.
Multiple meta-algorithm can be nested, allowing for easy creation of new algorithms.
- cusum_2_b cd_algorithm 30 0 30 5 cusum 30 0.1 25 0 9 round round 6 alg_with_exploration alg_with_exploration 0.01 2 ucb1 ucb1
 ---------- alg_name alg_id M use_history max_history length_of_cdt_specification cdt_specification length_of_subalg_specification subalg_specification
Other distributions specifications can be inferred reading the parser implementation, which is the get_algorithm function in utils.cpp.
