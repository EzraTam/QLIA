# Q-LEARNING WITH INFORMATION ASYMMETRY 

QLIA is a simulation, implementing the Q-Learning Algorithm used in the paper "On Information Asymmetry in Competitive Multi-Agent Reinforcement Learning: Convergence and Optimality". 
(https://arxiv.org/abs/2010.10901)

Two simulation folder are included, modelling the QLIA-Algorithm in case of standard and predictive learning in a STATIONARY and DYNAMIC environment.
In particular, we simulate two training phases - one for 'standard' asymmetrical Q-Learning and one for 'predictive' asymmetrical Q-Learning - as well as multiple testing phases for each case, which calculate the value function depending on each agents respective choice of either greedy or boltzman strategies.

## Functions

The folder 'MATLAB_FUNC' should contain the following MATLAB functions:
- analyse_result.m
- calculate_parameters.m
- compute_strategy.m
- custom_sampler.m
- get_GA_policy.m
- get_LA_policy.m
- sample_boltz.m

The folder 'QLAI_LIB' should contain the follwing headers
- algorithm_func.h
- q_learning_lib.h
- util_func.h

The folders 'Stationary' and 'Dynamic' should each contain their respective 'main.m' and 'execute_computation.c' C-File, which will be automatically compiled to a .MEX-File executable.

## Simulation Parameters

Simulation Parameters are to be specified inside the header file 'q_learning.h'. Additionally the parameters 'beta' and 'tau' are to be addtionally specified inside the 'main.m' function (see lines 19 and 20).

## Usage

WINDOWS:
Before simulation please make sure that You have already installed the MinGW-w64 C/C++ Compiler.
(https://de.mathworks.com/matlabcentral/fileexchange/52848-matlab-support-for-mingw-w64-c-c-compiler)

LINUX:
No further installation is needed. The .MEX-File is compiled using 'gcc'. 

BASIC SET UP:
Per default the simulation is set up to run from the Command Line/Console, i.e. start MATLAB using the option '-nodesktop'.
To run the simulation from the MATLAB GUI, please comment the following lines inside 'execute_computation.c':
- from 160 to 167
- from 299 to 306
- from 464 to 471

RUN SIMULATION:
Simply execute main.m in MATLAB either from the Command Line/Console or the MATLAB GUI.

OUTPUT:
You can follow the progress of Your simulation from the Command Line/Console. After code execution four plots are automatically generated, illustrating the evolution of the value functions for each agent (Local and Global) during the testing phase. 
Furthermore, the model, i.e. the Probability Transition Matrices and the randomly generated Reward Functions are transcribed to a .txt-File called 'MDP_DATA_SHEET.txt'. Should you be interested in re-using or analyzing the model, You can use the _RAW_DATA_SHEETs, which contain the same information but are easier to import. 

## Contributing
We welcome any ideas regarding improvement of code execution. Feel free to use the code as a basis for further development in Your own scientific research. In both cases, or should any issues or questions arise, please contact us at: 

ezra.tampubolon@tum.de 
haris.ceribasic@tum.de


