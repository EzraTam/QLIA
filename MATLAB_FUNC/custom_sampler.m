%% Function to sample element from a probability vector 
function sample = custom_sampler(vec)

interval = length(vec);
check = zeros(interval);
aux = rand;

check(1) = vec(1);

for i = 2:interval
    if aux <= check(i-1)
        sample = i-1;
        break;
    end
    check(i) = vec(i) + check(i-1);
    sample = i;
end
