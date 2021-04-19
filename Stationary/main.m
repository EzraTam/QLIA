%% ************** Q-Learning with Information-Asymmetry **************** %%
%% ************************* STATIONARY CASE *************************** %%
% This script implements a Q-Learning Algorithm in case of information
% asymmetry between a localized and globalized agent.
% It improves the computational power by outsourcing its calculations
% to a C-File called 'execute_computation.c'.
% 'execute_computation.c' can be called with five specifications:
% - 0 : Build Model (MDP and Reward structure) and store data sheets
% - 1 : Training Phase
% - 2 : Training Phase for Predictive learning
% - 3 : Testing Phase
% - 4 : Clean Up function to free memory
clear,clc;

% SET UP DIRECTORY
addpath('../MATLAB_FUNC');

disp("***************** Q-LEARNING WITH INOFRMATION-ASYMMETRY *****************")
disp("*************************** STATIONARY CASE *****************************")

% GLOBAL PARAMETERS
global S A_LOC A_GLO beta tau

beta = 0.8; % specify the same beta in MATLAB and C
tau = 1.3;  % specify the same beta in MATLAB and C

% DATA STRUCTURES
agent_local = struct;
agent_global = struct;

% BUILD MODEL
% Compile MEX-File
mex execute_computation.c -R2018a
disp("> FUNCTION CALL TO MEX-FILE: 'execute_computation.c'...")
execute_computation(0);

%% A) COMPUTATION
%% A.1) TRAINING PHASE STANDARD
disp("> FUNCTION CALL TO MEX-FILE: 'execute_computation.c'...")

[Q_LA,Q_GA] = execute_computation(1);
disp("> Reloading data to MATLAB has been succesfull!")

% GET PARAMETERS
[S,A_LOC] = size(Q_LA);
[~,A_GLO] = size(Q_GA);

% COMPUTE STRATEGY
disp('> Computing the Strategy-Profile of the LOCAL AGENT ...')
[~,agent_local.strategy_boltzman] = compute_strategy(Q_LA);

disp('> Computing the Strategy-Profile of the GLOBAL AGENT ...')
[agent_global.strategy_greedy,agent_global.strategy_boltzman] = compute_strategy(Q_GA);

% STORE Q-TABLES IN DATA STRUCTURE
agent_local.Q_table = Q_LA;
agent_global.Q_table = cell(1,A_LOC);

for i = 1:A_LOC
    agent_global.Q_table{1,i} = Q_GA((1 + (i-1)*S):(S + (i-1)*S),:);
end


%% A.2) TRAINING PHASE PREDICTIVE CASE
disp("> FUNCTION CALL TO MEX-FILE: 'execute_computation.c'...")

[Q_LA_Predictive,Q_GA_Predictive] = execute_computation(2);
disp("> Reloading data has been succesfull!")

% COMPUTE STRATEGY
disp('> Computing the Strategy-Profile of the LOCAL AGENT ...')
agent_local.strategy_boltzman_predictive = agent_local.strategy_boltzman;

disp('> Computing the Strategy-Profile of the GLOBAL AGENT ...')
[agent_global.strategy_greedy_predictive,agent_global.strategy_boltzman_predictive] = compute_strategy(Q_GA_Predictive);

% STORE Q-TABLES IN DATA STRUCTURE
agent_local.Q_table_predictive = Q_LA_Predictive;
agent_global.Q_table_predictive = cell(1,A_LOC);

for i = 1:A_LOC
    agent_global.Q_table_predictive{1,i} = Q_GA_Predictive((1 + (i-1)*S):(S + (i-1)*S),:);
end

% % DISPLAY Q-TABLES
% disp('--------------------------------------------')
% disp('Q-TABLE OF LOCAL AGENT:')
% disp(agent_local.Q_table);
% disp('--------------------------------------------')
%
% disp('Q-TABLE OF GLOBAL AGENT:')
% for key = 1:A_LOC
%     disp(agent_global.Q_table{1,key});
% end
% disp('--------------------------------------------')

%% STANDARD CASE
% TESTING PHASE
disp("> FUNCTION CALL TO MEX-FILE: 'execute_computation.c'...")
[reward_LA_BG,reward_GA_BG] = execute_computation(3,agent_local.strategy_boltzman,agent_global.strategy_greedy);

%% B) ANALYSIS AND VISUALIZATION
disp('**************************** STANDARD CASE *******************************')

% Parameters
iter_cap = 50;
t = linspace (0,iter_cap-1,iter_cap);    % Iteration vector


%% B.2) Global Agent
% Analyze Result
disp('GLOBAL AGENT:')
agent_global.V_BG = analyse_result(reward_GA_BG);

disp('|   V_BG   |');
disp([agent_global.V_BG])

disp('--------------------------------------------')

% Calculate Parameters
[agent_global.mean_BG,agent_global.CI95_BG] = calculate_parameters...
    (reward_GA_BG);


%% PREDICTIVE CASE
[reward_LA_BG_predictive,reward_GA_BG_predictive] = execute_computation(3,agent_local.strategy_boltzman_predictive,agent_global.strategy_greedy_predictive);

%% B) ANALYSIS AND VISUALIZATION
disp('*************************** PREDICTIVE CASE ******************************')

% Parameters
iter_cap = 50;
t = linspace (0,iter_cap-1,iter_cap);    % Iteration vector

%% B.2) Global Agent
% Analyze Result
disp('GLOBAL AGENT:')
agent_global.V_BG_predictive = analyse_result(reward_GA_BG_predictive);

disp('|   V_BG   |');
disp([agent_global.V_BG_predictive])

disp('----------------------------------------')

% Calculate Parameters
[agent_global.mean_BG_predictive,agent_global.CI95_BG_predictive] = calculate_parameters...
    (reward_GA_BG_predictive);

% Plot Result
figure('Name','Global Agent')
plot(t,agent_global.mean_BG(1:iter_cap),':','LineWidth',1.5)
hold on
plot(t,agent_global.mean_BG_predictive(1:iter_cap),'-.','LineWidth',1.5)
grid;
xlabel('iteration k')
ylabel('Value V')
legend({'Standard Case', 'Predictive Case'},'Location','southeast','FontSize',11)
disp('************************** SIMULATION FINISHED ***************************')

% FREE MEMORY
disp("> FUNCTION CALL TO MEX-FILE: 'execute_computation.c'...")
execute_computation(4);
