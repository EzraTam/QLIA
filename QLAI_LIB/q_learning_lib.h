#ifndef Q_LEARNING_LIB_H
#define Q_LEARNING_LIB_H

/* Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include "matrix.h"

// SIMULATION PARAMETERS
/* ***************************** COMMENTARY ******************************** */
// This File is the Header File of the Simulation, containing all PARAMETERS
// that are to be specified beforehand. Please enter them here:

/* State Action Space*/
#define A_LOC 5
#define A_GLO 5
#define S 5

/* Testing Parameters*/
#define iter_max_testing 100  // maximal iteration number during testing
#define sample_max 30000  // maximal sample number during testing

/* Parameters */
double beta = 0.8;  // Learning rate
double tau = 1.3; // Boltzman parameter
int iter_max_training = 3000000; // Maxmimal iteration number during training

/* ************************************************************************* */

/* Q_Tables */
double Q_LA[S][A_LOC];
double Q_GA[A_LOC][S][A_GLO];

/* Strategy */
double LA_strategy[S][A_LOC];
double GA_strategy[A_LOC][S][A_GLO];

/* Count Matrices */
int count_local[S][A_LOC];
int count_global[A_LOC][S][A_GLO];

/* Markov Desicion Model */
typedef struct MDP {
  double PTM[S][S];
  double reward_local[S];
  double reward_global[S];
  struct MDP* next;
} MDP;

/* MDP constructor */
MDP* createPTM(MDP* previous, int index);
MDP *start; // Start of the linked list is to be specified as a global varible !

/* Utility functions */
double * get_LA_vector(double matrix[S][A_LOC], int index);
double * get_GA_vector(double matrix[A_LOC][S][A_GLO], int index1, int index2);
int custom_sampler(double *vec, int size);
int sample_boltz(double *vec, int size);
double get_max(double *vec, int size);
int sub2ind(int a_loc, int a_glob);
int* ind2sub(int index);
MDP* get_pointer(MDP* start, int index);
double *get_MDP_vector(MDP *temp, int index);
double get_param(int count);

/* Algorithm Functions */
double get_LA_reward(int LA_policy, int GA_policy, int state); // optional
double get_GA_reward(int LA_policy, int GA_policy, int state); // optional
double update_Q(double Q_value, double Q_Nash, double reward, double alpha);

#endif
