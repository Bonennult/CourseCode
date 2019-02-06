function LSE
clear all;  close all;  clc;
% set parameters
global t;                   % threshold
global delta;               % tolerance
t = 1;
delta = 0.975;
% domain
global omega;  
global omegaX1;
global omegaX2;
height = 30;
width = 60;
omegaX1 = linspace(0,1,height);
omegaX2 = linspace(0,2,width);
[omegaX1, omegaX2] = meshgrid(omegaX1, omegaX2);
% omegaX1 = omegaX1(:);
% omegaX2 = omegaX2(:);
omega = [omegaX1(:), omegaX2(:)];
% for straddle
global U;
U = omega;
% noise variance
global sigma_noise;         % standard variance
sigma_noise = exp(-1);
% for selecting points
global gamma;
global epsilon;
global beta;
gamma = 1e-8;
epsilon = 1e-8;
beta = 1.96;
% initial sampled points
global GP;
global GP0;
global GP1;
global GP2;
max_iter = 40;
for m=1:1
    idx = randperm(length(omega), 3);
    GP = zeros(size(idx,1), 3);   % 2D synthetic
    U = omega;
    for i = 1:length(idx)
        GP(i,1:2) = omega(idx(i),:);
        GP(i,3) = f(omega(idx(i),:)) + noise(omega(idx(i),:));
        U(i,:) = [];
    end
    % Straddle-Multi
    iter = max_iter;
    while(iter>0)
        disp(iter);
%         disp(size(GP,1));
        % x_plus = SelectPoint(GP);
        x_plus = Straddle(GP, max_iter-iter+1, width, height, 0);
        y_plus = f(x_plus) + noise(x_plus);
        GP = [GP; x_plus, y_plus];
        disp([x_plus, y_plus]);
        iter = iter - 1;
    end
    GP0 = GP;
    % Straddle
    U = omega;
    for i = 1:length(idx)
        U(i,:) = [];
    end
    GP = GP(1:3,:);
    iter = max_iter;
    while(iter>0)
        disp(iter);
%         disp(size(GP,1));
        x_plus = Straddle(GP, max_iter-iter+1, width, height, 1);
        y_plus = f(x_plus) + noise(x_plus);
        GP = [GP; x_plus, y_plus];
        disp([x_plus, y_plus]);
        iter = iter - 1;
    end
    GP1 = GP;
    % RMILE
    GP = GP(1:3,:);
    iter = max_iter;
    while(iter>0)
        disp(iter);
%         disp(size(GP,1));
        x_plus = SelectPoint(GP);
        y_plus = f(x_plus) + noise(x_plus);
        GP = [GP; x_plus, y_plus];
        disp([x_plus, y_plus]);
        iter = iter - 1;
    end
    GP2 = GP;
    % plot
    name = fix(clock);
    save([num2str(name(2)),'-',num2str(name(3)),'-',num2str((name(4))),'-',num2str((name(5))),'.mat'],'GP0','GP1','GP2');
    % load('GP.mat','GP')
    % title = 'RMILE';
    tit = 'Straddle-Multi';
    plotLSE(height, width, tit);
    saveas(gca,[num2str(name(2)),'-',num2str(name(3)),'-',num2str((name(4))),'-',num2str((name(5))),'.png']);
    saveas(gca,[num2str(name(2)),'-',num2str(name(3)),'-',num2str((name(4))),'-',num2str((name(5))),'.eps'],'psc2');
    saveas(gca,[num2str(name(2)),'-',num2str(name(3)),'-',num2str((name(4))),'-',num2str((name(5))),'.fig']);
end
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
%% Select next sample point
function x_star = SelectPoint(GP_local)
global omega;
global delta;
global gamma;
global epsilon;
global t;
mu = mean_cond(GP_local, omega);
sigma = kernel_cond(GP_local, omega, omega);
mod_I = cdf('Normal', ones(length(mu),1)*(t-epsilon), mu, sigma);
mod_I = sum(mod_I <= 1 - delta);
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
%% Straddle
function x_star = Straddle(GP_local,iter, width, height, coeff)
global t;
global U;
% N = round((width+height)/10);
N = 9;
recent_sample = max(size(GP_local,1)-round(0.4*N), 1);
recent_sample = GP_local(recent_sample:end,3);
beta = Beta(GP_local, iter, width, height);
a = var(recent_sample);
b = beta * sqrt(kernel_cond(GP_local, U, U));
c = abs(mean_cond(GP_local, U) - t);
disp(a(1));
disp(b(2));
disp(c(3));
disp(beta);
if coeff
    straddle = 1.96 * sqrt(kernel_cond(GP_local, U, U)) - abs(mean_cond(GP_local, U) - t);
else
    straddle = beta * sqrt(kernel_cond(GP_local, U, U)) - abs(mean_cond(GP_local, U) - t)*var(recent_sample);% + var(recent_sample);
end
[~, idx] = max(straddle);
x_star = U(idx,:);
U(idx,:) = [];
end
% compute beta which is time-variant
function beta = Beta(GP_local, iter, width, height)
%N = round((width+height)/10);
N = 9;
recent_sample = max(size(GP_local,1)-round(0.4*N), 1);
recent_sample = GP_local(recent_sample:end,3);
beta = 1.96*0.05*log((pi*iter).^2);%*var(recent_sample);
% if mod(iter,N)>=1 && mod(iter,N)<round(0.5*N)
%     beta = 0.3*beta;
% end
end
%% Plot
function plotLSE(height, width, tit)
global omega;
global omegaX1;
global omegaX2;
global GP;
global GP0;
global GP1;
global GP2;
figure;
set(gcf,'outerposition',get(0,'screensize'));    % maximaze window
% suptitle(title);
hold on;
% ground truth contour
subplot(2,2,1);
title('Ground Truth');
hold on;
box on;
Z = f(omega);
Z = reshape(Z,[width, height]);
[C,h] = contour(omegaX1, omegaX2, Z);
clabel(C,h);
plot(GP(1:3,1),GP(1:3,2),'o','MarkerFaceColor','b');
for i=1:3          % mark sampling sequence number
    text(GP(i,1)+0.02,GP(i,2)+0.02,num2str(i),'FontSize',14,'FontWeight','Bold');
end
% save('gd.mat','omega', 'Z', 'omegaX1', 'omegaX2');
% implied contour:Straddle-Multi
subplot(2,2,2);
title('Straddle-Multi');
hold on;
box on;
Z = mean_cond(GP0, omega);
Z = reshape(Z,[width, height]);
[C,h] = contour(omegaX1, omegaX2, Z);
clabel(C,h);
plot(GP0(:,1),GP0(:,2),'o','MarkerFaceColor','b');
for i=1:size(GP0,1)          % mark sampling sequence number
    text(GP0(i,1)+0.02,GP0(i,2)+0.02,num2str(i),'FontSize',14,'FontWeight','Bold');
end
% Straddle
subplot(2,2,3);
title('Straddle');
hold on;
box on;
Z = mean_cond(GP1, omega);
Z = reshape(Z,[width, height]);
[C,h] = contour(omegaX1, omegaX2, Z);
clabel(C,h);
plot(GP1(:,1),GP1(:,2),'o','MarkerFaceColor','b');
for i=1:size(GP1,1)          % mark sampling sequence number
    text(GP1(i,1)+0.02,GP1(i,2)+0.02,num2str(i),'FontSize',14,'FontWeight','Bold');
end
% RMILE
subplot(2,2,4);
title('RMILE');
hold on;
box on;
Z = mean_cond(GP2, omega);
Z = reshape(Z,[width, height]);
[C,h] = contour(omegaX1, omegaX2, Z);
clabel(C,h);
plot(GP2(:,1),GP2(:,2),'o','MarkerFaceColor','b');
for i=1:size(GP2,1)          % mark sampling sequence number
    text(GP2(i,1)+0.02,GP2(i,2)+0.02,num2str(i),'FontSize',14,'FontWeight','Bold');
end
end