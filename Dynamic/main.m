%% ************** Q-Learning with Information-Asymmetry **************** %%
%% ***************************** DYNAMIC CASE ************************** %%
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
disp("******************************* DYNAMIC CASE ****************************")

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
[agent_local.strategy_greedy,agent_local.strategy_boltzman] = compute_strategy(Q_LA);

disp('> Computing the Strategy-Profile of the GLOBAL AGENT ...')
[agent_global.strategy_greedy,agent_global.strategy_boltzman] = compute_strategy(Q_GA);

% STORE Q-TABLES IN DATA STRUCTURE
agent_local.Q_table = Q_LA;
agent_global.Q_table = cell(1,A_LOC);

for i = 1:A_LOC
    agent_global.Q_table{1,i} = Q_GA((1 + (i-1)*S):(S + (i-1)*S),:);
end

% COMPUTE GREEDY POLICY
agent_local.policy = get_LA_policy(agent_local.strategy_greedy);
agent_global.policy = get_GA_policy(agent_local.policy,agent_global.Q_table);

%% A.2) TRAINING PHASE PREDICTIVE CASE
disp("> FUNCTION CALL TO MEX-FILE: 'execute_computation.c'...")

[Q_LA_Predictive,Q_GA_Predictive] = execute_computation(2);
disp("> Reloading data has been succesfull!")

% COMPUTE STRATEGY
disp('> Computing the Strategy-Profile of the LOCAL AGENT ...')
[agent_local.strategy_greedy_predictive,agent_local.strategy_boltzman_predictive] = compute_strategy(Q_LA_Predictive);

disp('> Computing the Strategy-Profile of the GLOBAL AGENT ...')
[agent_global.strategy_greedy_predictive,agent_global.strategy_boltzman_predictive] = compute_strategy(Q_GA_Predictive);

% STORE Q-TABLES IN DATA STRUCTURE
agent_local.Q_table_predictive = Q_LA_Predictive;
agent_global.Q_table_predictive = cell(1,A_LOC);

for i = 1:A_LOC
    agent_global.Q_table_predictive{1,i} = Q_GA_Predictive((1 + (i-1)*S):(S + (i-1)*S),:);
end

% COMPUTE GREEDY POLICY
agent_local.policy_predictive = get_LA_policy(agent_local.strategy_greedy_predictive);
agent_global.policy_predictive = get_GA_policy(agent_local.policy_predictive,agent_global.Q_table_predictive);


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

[reward_LA_GG,reward_GA_GG] = execute_computation(3,agent_local.strategy_greedy,agent_global.strategy_greedy);
[reward_LA_GB,reward_GA_GB] = execute_computation(3,agent_local.strategy_greedy,agent_global.strategy_boltzman);
[reward_LA_BG,reward_GA_BG] = execute_computation(3,agent_local.strategy_boltzman,agent_global.strategy_greedy);
[reward_LA_BB,reward_GA_BB] = execute_computation(3,agent_local.strategy_boltzman,agent_global.strategy_boltzman);

%% B) ANALYSIS AND VISUALIZATION
disp('**************************** STANDARD CASE *******************************')

% Parameters
iter_cap = 50;
t = linspace (0,iter_cap-1,iter_cap);    % Iteration vector

%% B.1) Local Agent
% Analyze result
disp('--------------------------------------------')
disp('LOCAL AGENT:')
agent_local.V_GG = analyse_result(reward_LA_GG);
agent_local.V_GB = analyse_result(reward_LA_GB);
agent_local.V_BG = analyse_result(reward_LA_BG);
agent_local.V_BB = analyse_result(reward_LA_BB);

disp('|   V_GG   |   V_GB  |   V_BG   |   V_BB  |');
disp([agent_local.V_GG,agent_local.V_GB,agent_local.V_BG,agent_local.V_BB])

disp('--------------------------------------------')

% Calculate parameters
[agent_local.mean_GG,agent_local.CI95_GG] = calculate_parameters...
    (reward_LA_GG);
[agent_local.mean_GB,agent_local.CI95_GB] = calculate_parameters...
    (reward_LA_GB);
[agent_local.mean_BG,agent_local.CI95_BG] = calculate_parameters...
    (reward_LA_BG);
[agent_local.mean_BB,agent_local.CI95_BB] = calculate_parameters...
    (reward_LA_BB);

% Plot Result
figure('Name','Local Agent')
plot(t,agent_local.mean_GG(1:iter_cap),'LineWidth',1.5)
hold on
plot(t,agent_local.mean_GB(1:iter_cap),':','LineWidth',1.5)
plot(t,agent_local.mean_BG(1:iter_cap),'--','LineWidth',1.5)
plot(t,agent_local.mean_BB(1:iter_cap),'-.','LineWidth',1.5)
grid;
xlabel('iteration k')
legend({'Greedy and Greedy','Greedy and Boltzman','Boltzman and Greedy'...
    ,'Boltzman and Boltzman'},'Location','southeast','FontSize',11)

%% B.2) Global Agent
% Analyze Result
disp('GLOBAL AGENT:')
agent_global.V_GG = analyse_result(reward_GA_GG);
agent_global.V_GB = analyse_result(reward_GA_GB);
agent_global.V_BG = analyse_result(reward_GA_BG);
agent_global.V_BB = analyse_result(reward_GA_BB);

disp('|   V_GG   |   V_GB  |   V_BG   |   V_BB  |');
disp([agent_global.V_GG,agent_global.V_GB,agent_global.V_BG,agent_global.V_BB])

disp('--------------------------------------------')

% Calculate Parameters
[agent_global.mean_GG,agent_global.CI95_GG] = calculate_parameters...
    (reward_GA_GG);
[agent_global.mean_GB,agent_global.CI95_GB] = calculate_parameters...
    (reward_GA_GB);
[agent_global.mean_BG,agent_global.CI95_BG] = calculate_parameters...
    (reward_GA_BG);
[agent_global.mean_BB,agent_global.CI95_BB] = calculate_parameters...
    (reward_GA_BB);

% Plot Result
figure('Name','Global Agent')
plot(t,agent_global.mean_GG(1:iter_cap),'LineWidth',1.5)
hold on
plot(t,agent_global.mean_BG(1:iter_cap),':','LineWidth',1.5)
plot(t,agent_global.mean_GB(1:iter_cap),'--','LineWidth',1.5)
plot(t,agent_global.mean_BB(1:iter_cap),'-.','LineWidth',1.5)
grid;
xlabel('iteration k')
ylabel('Value V')
legend({'Greedy and Greedy','Boltzman and Greedy','Greedy and Boltzman'...
    ,'Boltzman and Boltzman'},'Location','southeast','FontSize',11)

%% PREDICTIVE CASE
% TESTING PHASE
disp("> FUNCTION CALL TO MEX-FILE: 'execute_computation.c'...")
[reward_LA_GG_predictive,reward_GA_GG_predictive] = execute_computation(3,agent_local.strategy_greedy_predictive,agent_global.strategy_greedy_predictive);
[reward_LA_GB_predictive,reward_GA_GB_predictive] = execute_computation(3,agent_local.strategy_greedy_predictive,agent_global.strategy_boltzman_predictive);
[reward_LA_BG_predictive,reward_GA_BG_predictive] = execute_computation(3,agent_local.strategy_boltzman_predictive,agent_global.strategy_greedy_predictive);
[reward_LA_BB_predictive,reward_GA_BB_predictive] = execute_computation(3,agent_local.strategy_boltzman_predictive,agent_global.strategy_boltzman_predictive);

%% B) ANALYSIS AND VISUALIZATION
disp('*************************** PREDICTIVE CASE ******************************')

% Parameters
iter_cap = 50;
t = linspace (0,iter_cap-1,iter_cap);    % Iteration vector

%% B.1) Local Agent
% Analyze result
disp('LOCAL AGENT:')
agent_local.V_GG_predictive = analyse_result(reward_LA_GG_predictive);
agent_local.V_GB_predictive = analyse_result(reward_LA_GB_predictive);
agent_local.V_BG_predictive = analyse_result(reward_LA_BG_predictive);
agent_local.V_BB_predictive = analyse_result(reward_LA_BB_predictive);

disp('|   V_GG   |   V_GB  |   V_BG   |   V_BB  |');
disp([agent_local.V_GG_predictive,agent_local.V_GB_predictive,agent_local.V_BG_predictive,agent_local.V_BB_predictive])

disp('--------------------------------------------')

% Calculate parameters
[agent_local.mean_GG_predictive,agent_local.CI95_GG_predictive] = calculate_parameters...
    (reward_LA_GG);
[agent_local.mean_GB_predictive,agent_local.CI95_GB_predictive] = calculate_parameters...
    (reward_LA_GB);
[agent_local.mean_BG_predictive,agent_local.CI95_BG_predictive] = calculate_parameters...
    (reward_LA_BG);
[agent_local.mean_BB_predictive,agent_local.CI95_BB_predictive] = calculate_parameters...
    (reward_LA_BB);

% Plot Result
figure('Name','Local Agent - Predictive Case')
plot(t,agent_local.mean_GG_predictive(1:iter_cap),'LineWidth',1.5)
hold on
plot(t,agent_local.mean_GB_predictive(1:iter_cap),':','LineWidth',1.5)
plot(t,agent_local.mean_BG_predictive(1:iter_cap),'--','LineWidth',1.5)
plot(t,agent_local.mean_BB_predictive(1:iter_cap),'-.','LineWidth',1.5)
grid;
xlabel('iteration k')
legend({'Greedy and Greedy','Greedy and Boltzman','Boltzman and Greedy'...
    ,'Boltzman and Boltzman'},'Location','southeast','FontSize',11)

%% B.2) Global Agent
% Analyze Result
disp('GLOBAL AGENT:')
agent_global.V_GG_predictive = analyse_result(reward_GA_GG_predictive);
agent_global.V_GB_predictive = analyse_result(reward_GA_GB_predictive);
agent_global.V_BG_predictive = analyse_result(reward_GA_BG_predictive);
agent_global.V_BB_predictive = analyse_result(reward_GA_BB_predictive);

disp('|   V_GG   |   V_GB  |   V_BG   |   V_BB  |');
disp([agent_global.V_GG_predictive,agent_global.V_GB_predictive,agent_global.V_BG_predictive,agent_global.V_BB_predictive])

disp('----------------------------------------')

% Calculate Parameters
[agent_global.mean_GG_predictive,agent_global.CI95_GG_predictive] = calculate_parameters...
    (reward_GA_GG_predictive);
[agent_global.mean_GB_predictive,agent_global.CI95_GB_predictive] = calculate_parameters...
    (reward_GA_GB_predictive);
[agent_global.mean_BG_predictive,agent_global.CI95_BG_predictive] = calculate_parameters...
    (reward_GA_BG_predictive);
[agent_global.mean_BB_predictive,agent_global.CI95_BB_predictive] = calculate_parameters...
    (reward_GA_BB_predictive);

% Plot Result
figure('Name','Global Agent - Predictive Case')
plot(t,agent_global.mean_GG_predictive(1:iter_cap),'LineWidth',1.5)
hold on
plot(t,agent_global.mean_BG_predictive(1:iter_cap),':','LineWidth',1.5)
plot(t,agent_global.mean_GB_predictive(1:iter_cap),'--','LineWidth',1.5)
plot(t,agent_global.mean_BB_predictive(1:iter_cap),'-.','LineWidth',1.5)
grid;
xlabel('iteration k')
ylabel('Value V')
legend({'Greedy and Greedy','Boltzman and Greedy','Greedy and Boltzman'...
    ,'Boltzman and Boltzman'},'Location','southeast','FontSize',11)
disp('************************** SIMULATION FINISHED ***************************')

% FREE MEMORY
disp("> FUNCTION CALL TO MEX-FILE: 'execute_computation.c'...")
execute_computation(4);
