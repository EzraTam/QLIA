 %% Function to analyse the simulation result
 function V = analyse_result(reward)
 % @input:  - instantenious reward matrix over N Monte-Carlo iterations
 %          over time horizon iter_max
 % @output: - expected reward value V
 
 % Parameters
 global beta
 [N,iter_max] = size(reward);
 
 % Initialize
 cumulated_reward = zeros(N,1);
 aggregated_cumulated_reward = 0;
 
 % Cumulate reward over all iterations
 for i = 1:iter_max
     cumulated_reward = cumulated_reward + beta^(i-1)*reward(:,i);
 end
 
 % Aggregate reward over all samples
 for i = 1:N
     aggregated_cumulated_reward = aggregated_cumulated_reward ...
         + cumulated_reward(i);
 end
 
 % Calculate value
 V = 1/N * aggregated_cumulated_reward;