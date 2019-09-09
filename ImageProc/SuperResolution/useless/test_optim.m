clear all; close all; clc;
load Dictionary.mat
img = imread('2_lr.bmp');
ps = 3;     % patch size
lambda = 50*4*ps*ps;
% RGB空间变换到YCbCr空间
scale = 3;                       % 放大因子
YCC = rgb2ycbcr(img);
YCC_sr = imresize(YCC, scale);   % 默认bicubic插值
Y = YCC(:,:,1);                  % 取出亮度分量
% 取图像块
y = Y(ps+1:2*ps, 1:ps);
y_up = imresize(y, 2);
% 计算稀疏表达
lr = 0.00001;
maxstep = 300;
epsilon = 1e-5;
y = F(y_up);
alpha = randn(size(Dl,2), 1);
lastloss = lambda*norm(alpha,1) + 1/2*norm(Dl*alpha-y)^2;
%lastloss = lambda*sum(logical(alpha)) + 1/2*norm(Dl*alpha-y)^2;
gd = lambda*sign(alpha) + Dl'*(Dl*alpha-y);
%gd = lambda*logical(alpha) + Dl'*(Dl*alpha-y);
alpha = alpha - lr * gd;
loss = lambda*norm(alpha,1) + 1/2*norm(Dl*alpha-y)^2;
%loss = lambda*sum(logical(alpha)) + 1/2*norm(Dl*alpha-y)^2;
L = loss;
step = 0;
trainloss = zeros(1, maxstep);
while step < maxstep && abs(loss-lastloss)/loss > epsilon
    lastloss = loss;
    gd = lambda*sign(alpha) + Dl'*(Dl*alpha-y);
    %gd = lambda*logical(alpha) + Dl'*(Dl*alpha-y);
    alpha = alpha - lr * gd;
    loss = lambda*norm(alpha,1) + 1/2*norm(Dl*alpha-y)^2;
    %loss = lambda*sum(logical(alpha)) + 1/2*norm(Dl*alpha-y)^2;
    step = step+1;
    trainloss(step) = loss;
    rate = abs(loss-lastloss)/loss;
    %if loss < L/10
    %    lr = lr/10;
    %    L = loss;
    %end
end
plot(1:step,trainloss(1:step),'linewidth',2);
set(gca, 'fontsize', 16);
title(['lr=',num2str(lr),'  epsilon=', num2str(epsilon),...
    '  max step=',num2str(maxstep), '  iter step=',num2str(step)]);
diff = Dl*alpha-y;