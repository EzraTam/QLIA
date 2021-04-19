#ifndef ALGORITHM_FUNC_H
#define ALGORITHM_FUNC_H

#include "util_func.h"

/* ***************************** COMMENTARY ******************************** */
// This file contains all of the algorithmic functions used in Q-Learning.
// It should be noted, that the reward for the implemented simulation is randomly
// generated. For specific reward structures, edit the function below and go
// to 'util_func.h' to edit the MDP-constructor accordingly.
/* ************************************************************************* */

// Add custom reward function
/* Reward Function of the Local Agent */
double get_LA_reward(int LA_policy, int GA_policy, int state) {
  return 0.0;
}

/* Reward Function of the Global Agent */
double get_GA_reward(int LA_policy, int GA_policy, int state) {
  return 0.0;
}

/* Updating Function*/
double update_Q(double Q_value, double Q_Nash, double reward, double alpha) {
  return (1 - alpha)*Q_value + alpha*(reward + beta*Q_Nash);
}

#endif
