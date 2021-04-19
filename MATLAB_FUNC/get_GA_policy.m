function policy = get_GA_policy(LA_policy,Q_table)

global S

policy = zeros(S,1);

for s = 1:S
    [~,policy(s)] = max(Q_table{1,LA_policy(s)}(s,:));
end