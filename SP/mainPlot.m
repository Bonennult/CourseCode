function mainPlot()
global sigma_noise;         % standard variance
sigma_noise = exp(-1);
global omega;
global omegaX1;
global omegaX2;
height = 30;
width = 60;
omegaX1 = linspace(0,1,height);
omegaX2 = linspace(0,2,width);
[omegaX1, omegaX2] = meshgrid(omegaX1, omegaX2);
omega = [omegaX1(:), omegaX2(:)];

file = '2-4-16-37.mat';
load(file);

figure;
set(gcf,'outerposition',get(0,'screensize'));    % maximaze window
% suptitle(title);
hold on;
% ground truth contour
subplot(2,2,1);
set(gca, 'Position', get(gca, 'OuterPosition') - get(gca, 'TightInset') * [-1 0 1 0; 0 -1 0 1; 0 0 1 0; 0 0 0 1]);
title('Ground Truth');
hold on;
box on;
Z = f(omega);
Z = reshape(Z,[width, height]);
[C,h] = contour(omegaX1, omegaX2, Z);
clabel(C,h);
plot(GP0(1:3,1),GP0(1:3,2),'o','MarkerFaceColor','b');
for i=1:3          % mark sampling sequence number
    text(GP0(i,1)+0.02,GP0(i,2)+0.02,num2str(i),'FontSize',14,'FontWeight','Bold');
end
% implied contour:Straddle-Multi
subplot(2,2,2);
set(gca, 'Position', get(gca, 'OuterPosition') - get(gca, 'TightInset') * [-1 0 1 0; 0 -1 0 1; 0 0 1 0; 0 0 0 1]);
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
set(gca, 'Position', get(gca, 'OuterPosition') - get(gca, 'TightInset') * [-1 0 1 0; 0 -1 0 1; 0 0 1 0; 0 0 0 1]);
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

set(gca, 'Position', get(gca, 'OuterPosition') - get(gca, 'TightInset') * [-1 0 1 0; 0 -1 0 1; 0 0 1 0; 0 0 0 1]);
% saveas(gca,[file(1:end-4),'-2.eps'],'psc2');
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