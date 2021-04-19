#include "../QLAI_LIB/algorithm_func.h"
#include "mex.h"

/* ********************* The computational routine ************************** */
void execute_computation(int indicator, double (*LA_in)[S], double (*GA_in)[S*A_LOC], double (*Q_Table1)[S], double (*Q_Table2)[S*A_LOC], double (*LA_reward_table)[sample_max], double (*GA_reward_table)[sample_max])
{
    // Confirm, that call has been succesfull (optional)
    printf("Calling 'execute_computation.c' with specification %d has been succesfull!\n",indicator);

    // Initialize integer variables
    mwSize i,j,k,size,state,iter,local_policy,global_policy,state_new,num_sample;

/* ############################## BUILD MODEL ############################### */
/* ########################################################################## */

    if (indicator == 0) {

      printf("\n");
      printf("############################## BUILD MODEL ###############################\n");

      // Set seed for random number generator (only once!)
      srand(time(0));

      printf("> Initialize Markov Desicion Process ...\n");

      /* Initialize MDP */
      start = NULL;
      MDP *newest = NULL;
      int index = 0;

      size = A_LOC * A_GLO; // specify size of linked list

      for(i = 0; i < size; i++) {
        if (start == NULL) {
          start = createPTM(NULL,index);
          newest = start;
        } else {
          newest = createPTM(newest,index);
          newest->next = NULL;
        }
        index++;
      }

      printf("\033[0;32m");
      printf("> MODEL BUILD SUCCESFULL!\n");
      printf("\033[0m");

      /* ****************** CREATE COMPLETE MDP-DATA SHEET ****************** */
      MDP *temp = start;

      FILE *fp;
      fp = fopen("MDP_DATA_SHEET.txt","w+");
      index = 0;
      int *subscript;

      fprintf(fp,"###################################################################################\n");

      while (temp != NULL) {
        subscript = ind2sub(index); // get subscripts, i.e. the a_loc and a_glo
        fprintf(fp, "(a_loc,a_glo) = (%d,%d)\n",subscript[0]+1,subscript[1]+1);
        fprintf(fp,"---------------------------------------------------------------------------------\n");
        fprintf(fp, "PROBABILTY TRANSITION MATRIX:\n");
        for(int i = 0; i < S; i++) {
          for(int j = 0; j < S; j++) {
            fprintf(fp,"%lf ",temp->PTM[i][j]);
          }
          fprintf(fp,"\n");
        }
        fprintf(fp,"\n");
        fprintf(fp,"---------------------------------------------------------------------------------\n");

        fprintf(fp,"LOCAL REWARD: ");
        for(i = 0; i < S; i++) {
          fprintf(fp,"%lf ",temp->reward_local[i]);
        }
        fprintf(fp,"\n");

        fprintf(fp,"GLOBAL REWARD: ");
        for(i = 0; i < S; i++) {
          fprintf(fp,"%lf ",temp->reward_global[i]);
        }
        fprintf(fp,"\n");

        temp = temp->next;
        index++;

        fprintf(fp,"###################################################################################\n");
      }
      fclose(fp);

      /* ******************* CREATE RAW DATA SHEETS ************************ */
      temp = start;

      FILE *fpPTM;
      fpPTM = fopen("PTM_RAW_DATA.txt","w+");
      FILE *fpLA;
      fpLA = fopen("LA_RAW_DATA.txt","w+");
      FILE *fpGA;
      fpGA = fopen("GA_RAW_DATA.txt","w+");

      while (temp != NULL) {
        for(int i = 0; i < S; i++) {
          fprintf(fpLA,"%lf;",temp->reward_local[i]);
          fprintf(fpGA,"%lf;",temp->reward_global[i]);
          for(int j = 0; j < S; j++) {
            fprintf(fpPTM,"%lf ",temp->PTM[i][j]);
          }
          fprintf(fpPTM,"\n");
        }
        fprintf(fpGA,"\n");
        fprintf(fpLA,"\n");
        temp = temp->next;
      }
      fclose(fpPTM);
      fclose(fpLA);
      fclose(fpGA);
      free(temp);

      printf("> Model data has been written to 'MDP_DATA_SHEET.txt'\n");
      printf("##########################################################################\n");
      printf("\n");

    } else {

/* ############################ TRAINING PHASE ############################## */
/* ########################################################################## */

      if(indicator == 1) {

        printf("\n");
        printf("############################ TRAINING PHASE ##############################\n");

        MDP *temp = start;  // Needed Later, DO NOT COMMENT!

        printf("> Initialize Q-Tables ...\n");

        /* Initialize Q-table */
        for(i = 0; i < A_LOC; i++) {
          for(j = 0; j < S; j++) {
            for(k = 0; k < A_GLO; k++) {
              Q_GA[i][j][k] = 0;
            }
            Q_LA[j][i] = 0;
          }
        }
        printf("> Done!\n");

        /* Initialize Training */
        printf("> START TRAINING PHASE FOR %d ITERATIONS\n",iter_max_training);

        // Initial State
        state = 0;

        for (iter = 0; iter < iter_max_training; iter++) {

          /* ************************ COMMENTARY **************************** */
          // If you are running MATLAB from the GUI, please comment the following
          // lines

          int progress = round(((iter*1.0+1)/iter_max_training*1.0)*100);
          printf("\033[0;31m");
          if (iter == iter_max_training - 1) {
            printf("\033[0;32m");
          }
          mexPrintf("\rIn progress ... [%d%%]", progress);
          printf("\033[0m");
          fflush(stdout);

          /* **************************************************************** */


          /*
          printf("************************************************************* \n");
          printf("> Iteration Number: %d\n", iter + 1);
          */

          // SAMPLE ACTIONS
          local_policy = sample_boltz(get_LA_vector(Q_LA,state),A_LOC);
          global_policy = sample_boltz(get_GA_vector(Q_GA,local_policy,state),A_GLO);

          // Print result
          /*
          printf("> SAMPLED ACTIONS \n");
          printf("LA Action: %d\n", local_policy + 1);
          printf("GA Action: %d\n", global_policy + 1);
          */

          // MOVE TO NEW STATE
          temp = get_pointer(start,sub2ind(local_policy,global_policy));  // get pointer, which corresponds to the index (a_loc,a_glo)
          state_new = custom_sampler(get_MDP_vector(temp,state),S);
          // printf("NEW STATE: %d\n", state_new + 1);

          // UPDATE LOCAL Q-TABLE
          // printf("> Q-TABLES\n");
          // printf("----------------------- LOCAL AGENT -------------------------- \n");
          double alpha_LA = get_param(count_local[state][local_policy]);  // Calculate Learning Rate
          double local_reward = temp->reward_local[state];  // If randomized reward
          double Q_LA_Nash = get_max(get_LA_vector(Q_LA,state_new),A_LOC);
          Q_LA[state][local_policy] = update_Q(Q_LA[state][local_policy],Q_LA_Nash,local_reward,alpha_LA);
          /*
          printf("Reward = %lf, Q_LA_Nash = %lf\n", local_reward, Q_LA_Nash);
          for (i = 0; i < S; i++) {
            for(j = 0; j < A_LOC; j++) {
              printf("%lf ",Q_LA[i][j]);
            }
            printf("\n");
          }
          */

          // UPDATE GLOBAL Q-TABLE
          // printf("---------------------- GLOBAL AGENT --------------------------\n");
          double alpha_GA = get_param(count_global[local_policy][state][global_policy]); // Calculate Learning Rate
          double global_reward = temp->reward_global[state];  // If randomized reward
          double Q_GA_Nash = get_max(get_GA_vector(Q_GA,local_policy,state_new),A_GLO);
          Q_GA[local_policy][state][global_policy] = update_Q(Q_GA[local_policy][state][global_policy],Q_GA_Nash,global_reward,alpha_GA);
          /*
          printf("Reward = %lf, Q_GA_Nash = %lf\n", global_reward, Q_GA_Nash);
          for (i = 0; i < S; i++) {
            for(j = 0; j < A_GLO; j++) {
              printf("%lf ",Q_GA[local_policy][i][j]);
            }
            printf("\n");
          }
          printf("\n");
          printf("************************************************************* \n");
          */

          // RESET STATE AND INCREMENT COUNT
          state = state_new;
          count_local[state][local_policy]++;
          count_global[local_policy][state][global_policy]++;
        }
        printf("\n");
        printf("> TRAINING FINISHED! \n");

        printf("> Transcribing Data to MATLAB ...\n");

        /* Transcribe Solution to Output Matrices */
        for (int i = 0; i < S; i++) {
          for(int j = 0; j < A_LOC; j++) {
            Q_Table1[j][i] = Q_LA[i][j];
          }
        }

        for (int k = 0; k < A_LOC; k++) {
          for (int i = 0; i < S; i++) {
            for(int j = 0; j < A_GLO; j++) {
              Q_Table2[j][i+(k*S)] = Q_GA[k][i][j];
            }
          }
        }

        printf("> Done!\n");

        printf("##########################################################################\n");
        printf("\n");

      } else {


/* ####################### TRAINING PHASE PREDICTIVE ######################## */
/* ########################################################################## */

        if(indicator == 2) {

          printf("\n");
          printf("####################### TRAINING PHASE PREDICTIVE ########################\n");

          MDP *temp = start;

          printf("> Initialize Q-Tables ...\n");

          /* Initialize Q-table */
          for(i = 0; i < A_LOC; i++) {
            for(j = 0; j < S; j++) {
              for(k = 0; k < A_GLO; k++) {
                Q_GA[i][j][k] = 0;
                count_global[i][j][k] = 0;
              }
              Q_LA[j][i] = 0;
              count_local[j][i] = 0;
            }
          }
          printf("> Done!\n");

          /* Initialize Training */
          printf("> START TRAINING PHASE FOR %d ITERATIONS\n",iter_max_training);

          // Initialize state
          state = 0;
          double new_local_policy = 0;

          for (iter = 0; iter < iter_max_training; iter++) {

            /* ************************ COMMENTARY ************************** */
            // If you are running MATLAB from the GUI, please comment the following
            // lines.

            int progress = round(((iter*1.0+1)/iter_max_training*1.0)*100);
            printf("\033[0;31m");
            if (iter == iter_max_training - 1) {
              printf("\033[0;32m");
            }
            mexPrintf("\rIn progress ... [%d%%]", progress);
            printf("\033[0m");
            fflush(stdout);

            /* ************************************************************** */

            /*
            printf("************************************************************* \n");
            printf("> Iteration Number: %d\n", iter + 1);
            */

            // SAMPLE ACTIONS
            if (iter == 0) {
              local_policy = sample_boltz(get_LA_vector(Q_LA,state),A_LOC);
            } else {
              local_policy = new_local_policy;
            }

            global_policy = sample_boltz(get_GA_vector(Q_GA,local_policy,state),A_GLO);

            // Print result
            /*
            printf("> SAMPLED ACTIONS \n");
            printf("LA Action: %d\n", local_policy + 1);
            printf("GA Action: %d\n", global_policy + 1);
            */

            // MOVE TO NEW STATE
            temp = get_pointer(start,sub2ind(local_policy,global_policy));  // get pointer, which corresponds to the index (a_loc,a_glo)
            state_new = custom_sampler(get_MDP_vector(temp,state),S);
            // printf("NEW STATE: %d\n", state_new + 1);

            // UPDATE LOCAL Q-TABLE
            // printf("> Q-TABLES\n");
            // printf("----------------------- LOCAL AGENT -------------------------- \n");
            double alpha_LA = get_param(count_local[state][local_policy]);  // Calculate Learning Rate
            double local_reward = temp->reward_local[state];  // If randomized reward
            double Q_LA_Nash = get_max(get_LA_vector(Q_LA,state_new),A_LOC);
            Q_LA[state][local_policy] = update_Q(Q_LA[state][local_policy],Q_LA_Nash,local_reward,alpha_LA);
            /*
            printf("Reward = %lf, Q_LA_Nash = %lf\n", local_reward, Q_LA_Nash);
            for (i = 0; i < S; i++) {
              for(j = 0; j < A_LOC; j++) {
                printf("%lf ",Q_LA[i][j]);
              }
              printf("\n");
            }
            */

            // SAMPLE NEW LOCAL POLICY
            new_local_policy = sample_boltz(get_LA_vector(Q_LA,state_new),A_LOC);

            // UPDATE GLOBAL Q-TABLE
            // printf("---------------------- GLOBAL AGENT --------------------------\n");
            double alpha_GA = get_param(count_global[local_policy][state][global_policy]); // Calculate Learning Rate
            double global_reward = temp->reward_global[state];  // If randomized reward
            double Q_GA_Nash = get_max(get_GA_vector(Q_GA,new_local_policy,state_new),A_GLO);
            Q_GA[local_policy][state][global_policy] = update_Q(Q_GA[local_policy][state][global_policy],Q_GA_Nash,global_reward,alpha_GA);
            /*
            printf("Reward = %lf\n", global_reward);
            for (i = 0; i < S; i++) {
              for(j = 0; j < A_GLO; j++) {
                printf("%lf ",Q_GA[local_policy][i][j]);
              }
              printf("\n");
            }
            printf("\n");
            printf("************************************************************* \n");
            */

            // RESET STATE AND INCREMENT COUNT
            state = state_new;
            count_local[state][local_policy]++;
            count_global[local_policy][state][global_policy]++;
          }
          printf("\n");
          printf("> TRAINING FINISHED! \n");

          printf("> Transcribing Data to MATLAB ...\n");

          /* Transcribe Solution to Output Matrices */
          for (int i = 0; i < S; i++) {
            for(int j = 0; j < A_LOC; j++) {
              Q_Table1[j][i] = Q_LA[i][j];
            }
          }

          for (int k = 0; k < A_LOC; k++) {
            for (int i = 0; i < S; i++) {
              for(int j = 0; j < A_GLO; j++) {
                Q_Table2[j][i+(k*S)] = Q_GA[k][i][j];
              }
            }
          }

          printf("> Done!\n");

          printf("##########################################################################\n");
          printf("\n");

        } else {

/* ############################# TESTING PHASE ############################## */
/* ########################################################################## */

          if (indicator == 3) {

            printf("\n");
            printf("############################# TESTING PHASE ##############################\n");

            printf("> Import Strategy Data from MATLAB...\n");
            /* Initialize strategies */
            for (i = 0; i < S; i++) {
              for (j = 0; j < A_LOC; j++) {
                LA_strategy[i][j] = LA_in[j][i];
              }
            }

            for (k = 0; k < A_LOC; k++) {
              for (i = 0; i < S; i++) {
                for (j = 0; j < A_GLO; j++) {
                  GA_strategy[k][i][j] = GA_in[j][i+(k*S)];
                }
              }
            }

            printf("> Done!\n");

	          /*
            printf("----------------------- LOCAL AGENT -------------------------- \n");
            for (int i = 0; i < S; i++) {
              for(int j = 0; j < A_LOC; j++) {
                printf("%lf ",LA_strategy[i][j]);
              }
              printf("\n");
            }

            printf("---------------------- GLOBAL AGENT --------------------------\n");
            for (int k = 0; k < A_LOC; k++) {
            for (int i = 0; i < S; i++) {
              for(int j = 0; j < A_GLO; j++) {
                printf("%lf ",GA_strategy[k][i][j]);
              }
              printf("\n");
            }
            printf("\n");
          }
	         */

           MDP *temp = start;  // Needed Later, DO NOT COMMENT!

           /* Initialize Testing */
           printf("> TESTING FOR %d ITERATIONS %d SAMPLES ...\n", iter_max_testing, sample_max);

           for (num_sample = 0; num_sample < sample_max; num_sample++) {

             /* *********************** COMMENTARY ************************** */
             // If you are running MATLAB from the GUI, please comment the following
             // lines

             int progress = round(((num_sample*1.0+1)/sample_max*1.0)*100);
             printf("\033[0;31m");
             if (num_sample == sample_max - 1) {
               printf("\033[0;32m");
             }
             mexPrintf("\rIn progress ... [%d%%]", progress);
             printf("\033[0m");
             fflush(stdout);

             /* ************************************************************* */

             // Initial State
             state = 0;

             for (iter = 0; iter < iter_max_testing; iter++) {

               // SAMPLE ACTIONS
               local_policy = custom_sampler(get_LA_vector(LA_strategy,state),A_LOC);
               global_policy = custom_sampler(get_GA_vector(GA_strategy,local_policy,state),A_GLO);

               // MOVE TO NEW STATE
               temp = get_pointer(start,sub2ind(local_policy,global_policy));  // get pointer, which corresponds to the index (a_loc,a_glo)
               state_new = custom_sampler(get_MDP_vector(temp,state),S);

               // GET REWARD
               LA_reward_table[iter][num_sample] = temp->reward_local[state];
               GA_reward_table[iter][num_sample] = temp->reward_global[state];

               // RESET STATE
               state = state_new;
             }
           }

           printf("\n");
           printf("> TESTING FINISHED\n");

           printf("##########################################################################\n");
           printf("\n");

          } else {

/* ############################## FREE MEMORY ############################### */
/* ########################################################################## */

            printf("Freeing Memory ...\n");

            MDP *freeMe = start;
            MDP *holdMe = NULL;

            while(freeMe != NULL) {
              holdMe = freeMe->next;
              free(freeMe);
              freeMe = holdMe;
            }

            start = NULL;

            printf("> Done!\n");

          }
        }
      }
    }
}

/* ******************** The gateway function ******************************* */
void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    /* variable declarations here */
    mwSize indicator; // Indicator, specifying the code execution
    double (*inMatrix1)[S]; // Strategy Profile of LA of size S x A_LOC
    double (*inMatrix2)[S*A_LOC]; // Strategy Profile of GA of size (S*A_LOC) x A_GLO
    double (*outMatrix1)[S];  // Q-Table of LA of size S x A_LOC
    double (*outMatrix2)[S*A_LOC];  // Q-Table of GA of size (S*A_LOC) x A_GLO
    double (*outMatrix3)[sample_max]; // Reward Table of LA
    double (*outMatrix4)[sample_max]; // Reward Table of GA

    // Initialize indicator
    indicator = mxGetScalar(prhs[0]);

    /* check for proper number of arguments */
    if (indicator == 1 || indicator == 2) {
      if(nrhs!=1) {
          mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nrhs","One input required.");
      }
       if(nlhs!=2) {
          mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nlhs","Two outputs required.");
       }
    } else {
      if (indicator == 3) {
        if(nrhs!=3) {
            mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nrhs","Three inputs required.");
          }

          if(nlhs!=2) {
            mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nlhs","Two outputs required.");
          }
        } else {
          if(nrhs!=1) {
              mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nrhs","One input required.");
          }
        }
      }

     /* create the output matrices */
     // Depending on the indicator, different inputs/outputs are to be declared
     if (indicator == 1 || indicator == 2) {
       plhs[0] = mxCreateDoubleMatrix(S,A_LOC,mxREAL);
       outMatrix1 = (double(*)[S])mxGetData(plhs[0]);
       plhs[1] = mxCreateDoubleMatrix(S*A_LOC,A_GLO,mxREAL);
       outMatrix2 = (double(*)[S*A_LOC])mxGetData(plhs[1]);
     } else {
       if (indicator == 3) {
         inMatrix1 = (double(*)[S])mxGetData(prhs[1]);
         inMatrix2 = (double(*)[S*A_LOC])mxGetData(prhs[2]);
         plhs[0] = mxCreateDoubleMatrix(sample_max,iter_max_testing,mxREAL);
         outMatrix3 = (double(*)[sample_max])mxGetData(plhs[0]);
         plhs[1] = mxCreateDoubleMatrix(sample_max,iter_max_testing,mxREAL);
         outMatrix4 = (double(*)[sample_max])mxGetData(plhs[1]);
       }
    }

    /* call the computational routine */
    execute_computation(indicator,inMatrix1,inMatrix2,outMatrix1,outMatrix2,outMatrix3,outMatrix4);

}
