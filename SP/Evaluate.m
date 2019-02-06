function Evaluate()
clear all;  close all;  clc;
load('gd.mat');
t = 1;
height = 30;
width = 60;
global sigma_noise;         % standard variance
sigma_noise = exp(-1);
files = dir(fullfile('.','*.mat'));
files = files(end-3);
for i=1:length(files)
    load(files(i).name);
    Z1 = mean_cond(GP0, omega);
    Z1 = reshape(Z1,[width, height]);
    precision = sum(sum((Z1-t>0) .* (Z-t>0))) / sum(sum((Z1-t>0)));  % TP/(TP+FP)
    recall = sum(sum((Z1-t>0) .* (Z-t>0))) / sum(sum((Z1-t>0).*(Z-t>0) + (Z1-t<0).*(Z-t>0)));  % TP/(TP+FN)
    F1 = 2 * precision * recall / (precision + recall);
    if i==1
        result.name = files(i).name; 
        result.StraddleMulti = F1;
    else
        result(i).name = files(i).name;
        result(i).StraddleMulti = F1;
    end
    
    Z1 = mean_cond(GP1, omega);
    Z1 = reshape(Z1,[width, height]);
    precision = sum(sum((Z1-t>0) .* (Z-t>0))) / sum(sum((Z1-t>0)));  % TP/(TP+FP)
    recall = sum(sum((Z1-t>0) .* (Z-t>0))) / sum(sum((Z1-t>0).*(Z-t>0) + (Z1-t<0).*(Z-t>0)));  % TP/(TP+FN)
    F1 = 2 * precision * recall / (precision + recall);
    if i==1
        result.Straddle = F1;
    else
        result(i).Straddle = F1;
    end
    
    Z1 = mean_cond(GP2, omega);
    Z1 = reshape(Z1,[width, height]);
    precision = sum(sum((Z1-t>0) .* (Z-t>0))) / sum(sum((Z1-t>0)));  % TP/(TP+FP)
    recall = sum(sum((Z1-t>0) .* (Z-t>0))) / sum(sum((Z1-t>0).*(Z-t>0) + (Z1-t<0).*(Z-t>0)));  % TP/(TP+FN)
    F1 = 2 * precision * recall / (precision + recall);
    if i==1
        result.RMILE = F1;
    else
        result(i).RMILE = F1;
    end
end
save('result2.mat','result');
end
%% initial mean function
function mu = mean_init(x)
mu = 0 * ones(size(x,1),1);
end
% kernel function
function k = kernel_init(x1, x2)
sigma_ker = exp(1);
l = exp(-1.5);
k = zeros(size(x1,1),1);
for i = 1:length(k)
    k(i) = sigma_ker.^2 * exp(- sum((x1(i,:)-x2(i,:)).^2) / (2*l.^2));
end
end
%% conditional distribution
% compute Kn
function K_n = Kn(GP_local)
K_n = zeros(size(GP_local,1), size(GP_local,1));
for i=1:size(K_n,1)
    for j=1:size(K_n,2)
        K_n(i,j) = kernel_init(GP_local(i,1:2), GP_local(j,1:2));
    end
end
end
% conditional mean
function mu = mean_cond(GP_local, x)
global sigma_noise;
mu = zeros(size(x,1),1);
temp_K = (Kn(GP_local) + sigma_noise.^2 * eye(size(Kn(GP_local),1)));
for i=1:length(mu)
    kn = kernel_init(ones(size(GP_local,1),1)*x(i,:), GP_local(:,1:2));
    mu(i) = mean_init(x(i,:)) + kn' / temp_K * (GP_local(:,3) - mean_init(GP_local(:,1:2)));
end
end
% conditional kernel
function k = kernel_cond(GP_local, x1, x2)
global sigma_noise;
k = zeros(size(x1,1),1);
temp_K = (Kn(GP_local) + sigma_noise.^2 * eye(size(Kn(GP_local),1)));
for i = 1:size(k,1)
    kn1 = kernel_init(ones(size(GP_local,1),1)*x1(i,:), GP_local(:,1:2));
    kn2 = kernel_init(ones(size(GP_local,1),1)*x2(i,:), GP_local(:,1:2));
    k(i) = kernel_init(x1(i,:),x2(i,:)) - kn1' / temp_K  * kn2;
end
end