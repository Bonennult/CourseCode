function LSE
clear all;  close all;  clc;
% set parameters
global t;              % threshold
global delta;          % tolerance
t = 1;
delta = 0.975;
% domain
global omega;
global omegaX1;
global omegaX2;
omegaX1 = linspace(0,1,30);
omegaX2 = linspace(0,2,60);
[omegaX1, omegaX2] = meshgrid(omegaX1, omegaX2);
omegaX1 = omegaX1(:);
omegaX2 = omegaX2(:);
omega = [omegaX1, omegaX2];
% noise variance
global sigma_noise;    % standard variance
sigma_noise = exp(-1);
% for selecting point
global gamma;
global epsilon;
global beta;
gamma = 1e-8;
epsilon = 1e-8;
beta = 1.96;
% initial sampled points
idx = randperm(length(omega), 3);
GP = zeros(size(idx,1), 3);   % 2D synthetic
for i = 1:length(idx)
    GP(i,1:2) = omega(idx(i),:);
    GP(i,3) = f(omega(idx(i),:)) + noise(omega(idx(i),:));
end
% LSE
iter = 5;
while(iter>0)
    disp(iter);
    disp(size(GP,1));
    x_plus = SelectPoint(GP);
    y_plus = f(x_plus) + noise(x_plus);
    GP = [GP; x_plus, y_plus];
    disp([x_plus, y_plus]);
    iter = iter - 1;
end
% plot
plotLSE();
end
%% functions utilized
function y = f(x)
y = sin(10*x(:,1)) + cos(4*x(:,2)) - cos(3*x(:,1).*x(:,2));
end
% sample noise
function n = noise(x)
global sigma_noise;
n = zeros(size(x,1),1);
for i = 1:size(x,1)
n(i) = normrnd(0, sigma_noise);
end
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
mu = zeros(size(x,1),1);
temp_K = (Kn(GP_local) + eye(size(Kn(GP_local),1)));
for i=1:length(mu)
    kn = kernel_init(ones(size(GP_local,1),1)*x(i,:), GP_local(:,1:2));
    mu(i) = mean_init(x(i,:)) + kn' / temp_K * (GP_local(:,3) - mean_init(GP_local(:,1:2)));
end
end
% conditional kernel
function k = kernel_cond(GP_local, x1, x2)
k = zeros(size(x1,1),1);
temp_K = (Kn(GP_local) + eye(size(Kn(GP_local),1)));
for i = 1:size(k,1)
    kn1 = kernel_init(ones(size(GP_local,1),1)*x1(i,:), GP_local(:,1:2));
    kn2 = kernel_init(ones(size(GP_local,1),1)*x2(i,:), GP_local(:,1:2));
    k(i) = kernel_init(x1(i,:),x2(i,:)) - kn1' / temp_K  * kn2;
end
% if mode == 1         % convariance matrix
%     k = zeros(size(x1,1), size(x2,1));
%     temp_K = (Kn(GP_local) + eye(size(Kn(GP_local))));
%     for i = 1:size(k,1)
%         for j = 1:size(k,2)
%             kn1 = kernel_init(ones(size(GP_local,1),1)*x1(i,:), GP_local(:,1:2));
%             kn2 = kernel_init(ones(size(GP_local,1),1)*x2(j,:), GP_local(:,1:2));
%             k(i,j) = kernel_init(x1(i,:),x2(j,:)) - kn1' / temp_K  * kn2;
%         end
%     end
% elseif mode == 2     % variance array
%     k = zeros(size(x1,1),1);
%     temp_K = (Kn(GP_local) + eye(size(Kn(GP_local))));
%     for i = 1:size(k,1)
%         kn1 = kernel_init(ones(size(GP_local,1),1)*x1(i,:), GP_local(:,1:2));
%         kn2 = kernel_init(ones(size(GP_local,1),1)*x2(i,:), GP_local(:,1:2));
%         k(i) = kernel_init(x1(i,:),x2(i,:)) - kn1' / temp_K  * kn2;
%     end
% end
end
%% Acquisition function
function temp = E(GP_local, x_plus)
global omega;
global beta;
global sigma_noise;
global t;
temp = sqrt(kernel_cond(GP_local, x_plus, x_plus) + sigma_noise.^2);
temp = temp * (mean_cond(GP_local, omega) - beta * sqrt(kernel_cond([GP_local;x_plus,0], omega, omega)) - t);
temp = temp ./ abs(kernel_cond(GP_local, omega, ones(size(omega,1),1)*x_plus));
% disp(temp)
temp = cdf('Normal', temp, 0, 1);
temp = sum(temp);
end
%% select next sample point
function x_star = SelectPoint(GP_local)
global omega;
global delta;
global gamma;
global epsilon;
global t;
mu = mean_cond(GP_local, omega);
sigma = kernel_cond(GP_local, omega, omega);
mod_I = 1 - cdf('Normal', ones(length(mu),1)*(t-epsilon), mu, sigma);
mod_I = sum(mod_I > delta);
for i = 1:size(omega,1)
    temp_error = E(GP_local, omega(i,:)) - mod_I;
    temp_error = max(temp_error, gamma*sqrt(kernel_cond(GP_local, omega(i,:), omega(i,:))));
    if i == 1
        max_error = temp_error;
        x_star = omega(i,:);
    end
    if temp_error > max_error
        max_error = temp_error;
        x_star = omega(i,:);
    end
end
end
%% Plot
function plotLSE()
global omega;
global omegaX1;
global omegaX2;
global GP;
figure;
hold on;
Z = f(omega);
Z = reshape(Z,[length(omegaX1), length(omegaX2)]);
contour(omegaX1, omegaX2, Z);
plot(GP(:,1),GP(:,2),'o');
end