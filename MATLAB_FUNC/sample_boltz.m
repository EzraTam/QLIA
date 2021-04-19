%% Function to sample element from vector using Boltzman exploration
function [samp_vec,sample_index] = sample_boltz(vec)

global tau

denum = sum(exp(vec/tau));
samp_vec = exp(vec/tau)/denum;

sample_index = custom_sampler(samp_vec); 
