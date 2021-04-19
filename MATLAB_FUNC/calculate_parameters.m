%% Function to calculate mean and 95% confidence interval from sampled reward
function [mean_reward,CI95] = calculate_parameters(reward)

global beta 
[N,iter_max] = size(reward);

% Initialize
cumulated_reward = zeros(N,iter_max);
cumulated_reward(:,1) = reward(:,1);
mean_reward = zeros(1,iter_max);

for i = 2:iter_max
cumulated_reward(:,i) = cumulated_reward(:,i-1) + beta^(i-1)*reward(:,i);
end

% Calculate mean
for i = 1:iter_max
    mean_reward(i) = mean(cumulated_reward(:,i));
    SEM(i) = std(cumulated_reward(:,i))/sqrt(N);  % Standard Error Of The Mean
    CI95(i) = SEM(i) * tinv(0.975, N-1);            % 95% Confidence Intervals
end
