function policy = get_LA_policy(strategy_greedy)

[row,~] = size(strategy_greedy);

policy = zeros(row,1);

for i = 1:row
    [~,policy(i)] = max(strategy_greedy(i,:));
end