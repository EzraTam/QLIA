%% OBSERVE STRATEGY PROFILE
function [strategy_greedy,strategy_boltzman] = compute_strategy(Q_table)


% 1) Greedy strategy
strategy_greedy = zeros(size(Q_table));  % Allocate memory
for s = 1:length(Q_table)
    [~,index] = max(Q_table(s,:)); % Greedy strategy
    strategy_greedy(s,index) = 1;
end

% 2) Boltzman exploration
strategy_boltzman = zeros(size(Q_table));
for s = 1:length(Q_table)
     strategy_boltzman(s,:) = sample_boltz(Q_table(s,:));  % Boltzman Exploration
end

disp("> Done!")
