#ifndef UTIL_FUNC_H
#define UTIL_FUNC_H

#include "q_learning_lib.h"

/* ***************************** COMMENTARY ******************************** */
// This File contains all of the utility function used to build the simulation
// model and execute computational subroutines
/* ************************************************************************* */


/* Function, which returns a row vector of specified index for the Q_LA Matrix*/
double * get_LA_vector(double matrix[S][A_LOC], int index) {
  // IMPORTANT: Declare the local variable as static
  static double vec[A_LOC];
  int i;

  for (i = 0; i < A_LOC; i++) {
     vec[i] = matrix[index][i];
  }

  return vec;
}

/* Function, which returns a row vector of specified indices for the Q_GA Tensor*/
double * get_GA_vector(double matrix[A_LOC][S][A_GLO], int index1, int index2) {
  // IMPORTANT: Declare the local variable as static
  static double vec[A_GLO];
  int i;

  for (i = 0; i < A_GLO; i++) {
      vec[i] = matrix[index1][index2][i];
  }

  return vec;
}

/* Sample Index out of probability vector */
int custom_sampler(double *vec, int size) {
  double check[size];
  int sample = 0;

  double temp = ((double) rand() / (RAND_MAX));

  // Set the first check-value
  check[0] = vec[0];

  for (int i = 1; i < size; i++){
    if (temp <= check[i - 1]){
      return i - 1;
    }
    else {
      check[i] = vec[i] + check[i - 1];
      sample = i;
    }
  }

  // Sampled Index
  return sample;
}

/* Boltzman Exploration */
int sample_boltz(double *vec, int size) {
  double denum = 0; // Denominator
  double samp_vec[size];  // Sampled vector

  for (int i = 0; i < size; i++) {
    denum += exp(*(vec+i)/tau);
  }

  for (int i = 0; i < size; i++) {
    samp_vec[i] = exp(*(vec+i)/tau)/denum;
  }

  // Get sample form probibilty vector samp_vec
  int sample = custom_sampler(samp_vec,size);

  return sample;
}

/* Determine maximal Value of array */
double get_max(double *vec, int size) {
  int i, index = 0;

  for (i = 1; i < size; i++) {
    if (*(vec+i) > *(vec+index)) {
      index = i;
    }
  }

  return *(vec+index);
}

/* Convert subscript to linear index */
int sub2ind(int a_loc, int a_glob) {
  int index = 0;

  for(int i = 0; i < A_GLO; i++) {
    for(int j = 0; j < A_LOC; j++) {
      if (a_loc == j && a_glob == i) {
        return index;
      } else {
        index++;
      }
    }
  }

  // Safety mechansim: An error will occure since negative indices are not possible
  return -1;
}

/* Covert linear index to subscripts */
int* ind2sub(int index) {
  int aux = 0;
  static int vec[2];

  for(int i = 0; i < A_GLO; i++) {
    for(int j = 0; j < A_LOC; j++) {
      if(aux == index) {
        vec[0] = j;
        vec[1] = i;
        return vec;
      }
      aux++;
    }
  }

  return vec;
}


/* Constructor for MDP */
MDP* createPTM(MDP* previous, int index) {
  // Allocate Memory for new entry
  MDP* temp = (MDP*)malloc(sizeof(MDP));

  /* Initialize Probability Transition Matrix*/
  // Vector wich tracks the row-sum
  double sum_vec[S];

  for(int i = 0; i < S; i++) {
    double norm = 0;
    for(int j = 0; j < S; j++) {
      double aux = ((double) rand() / (RAND_MAX));  // random number between 0 and 1
      temp->PTM[i][j] = aux;
      norm += aux;
    }
    sum_vec[i] = norm;
  }

  // Normalize each row, by dividing each entry with the corresponding row-sum
  for(int i = 0; i < S; i++) {
    for(int j = 0; j < S; j++) {
      temp->PTM[i][j] = temp->PTM[i][j]/sum_vec[i];
    }
  }

  /* Initialize Reward Structure*/
  // The reward is uniformly distributed
  for(int i = 0; i < S; i++) {
    // The reward is uniformly distributed
    temp->reward_local[i] = -2.0 + 2.5 *((double) rand() / (RAND_MAX));
    temp->reward_global[i] = - temp->reward_local[i]; // Zero sum game
  }

  /* *************************** COMMENTARY ******************************* */
  // For specific reward function comment the for-loop above and uncomment the
  // following:
  /*
  int *subscript;
  subscript = ind2sub(index);
  for(int i = 0; i < S; i++) {
    temp->reward_local[i] = get_LA_reward(subscript[0], subscript[1], i);
    temp->reward_global[i] = get_GA_reward(subscript[0], subscript[1], i);
  }
  */
  /* ********************************************************************** */

  // Establish Link
  if(previous != NULL) {
    previous->next = temp;
  }

  return temp;
}

/* Find MDP with specified index */
MDP* get_pointer(MDP* start, int index) {
  MDP *temp = start;
  for (int i = 0; i < index; i++) {
    temp = temp->next;
  }
  return temp;
}

/* Function, which returns a row vector of specified index for the PTM Matrix*/
double *get_MDP_vector(MDP *temp, int index) {
  // IMPORTANT: Declare the local variable as static
  static double vec[S];
  int i;

  for (i = 0; i < S; i++) {
     vec[i] = temp->PTM[index][i];
  }
  return vec;
}

/* Calculate step size from count variable */
double get_param(int count) {
  return 10.0/(15.0 + count);
}

#endif
