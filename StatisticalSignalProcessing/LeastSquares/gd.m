% 基于 GD 的推荐算法
clear all; close all; clc;
load data_train.mat;
%% hyperparameters
rank = 10;
bound = [12, 30, 240, 600];
lrs = [0.00015, 0.00013, 0.00011, 0.00011];
lr = 0.0002;
lambda = 10;
epsilon = 0.00001;        % limit of iteration
%% learn
U = randn(943, rank);
V = randn(1682, rank);
epoch = 5;
%% randomly select train data and test data
idx = randperm(90000,80000);
train = data_train(idx,:);
temp = 1:90000;
temp(idx) = 0;
temp(temp==0) = [];
test = data_train(temp,:);
%% train
M = zeros(943, 1682);
W = zeros(943, 1682);
M(sub2ind(size(M), train(:,1), train(:,2))) = train(:,3);
W(sub2ind(size(W), train(:,1), train(:,2))) = 1;
step = 0;
train_loss = zeros(1001);
% 刚开始量级分别为 1e07, 1e04, 1e05
lossLast = norm(W.*(M-U*V'), 'fro').^2 + lambda/2*(norm(U,'fro').^2 + norm(V,'fro').^2);
gradU = 2*W.*(U*V'-M)*V + lambda*U;  % 刚开始 ~100 量级
lastGradU = gradU;
U = U - lr * gradU;
gradV = 2*(U'*(W.*(U*V'-M)))' + lambda*V;
lastGradV = gradV;
V = V - lr * gradV;
lossNew = norm(W.*(M-U*V'), 'fro').^2 + lambda/2*(norm(U,'fro').^2 + norm(V,'fro').^2);
while step<1000 && abs(lossLast-lossNew)/lossNew>epsilon
    lossLast = lossNew;
    gradU = 2*W.*(U*V'-M)*V + lambda*U;
    lastGradU = mean(abs(gradU(:)));
    U = U - lr * gradU;
    gradV = 2*(U'*(W.*(U*V'-M)))' + lambda*V;
    lastGradV = mean(abs(gradV(:)));
    V = V - lr * gradV;
    lossNew = norm(W.*(M-U*V'), 'fro').^2 + lambda/2*(norm(U,'fro').^2 + norm(V,'fro').^2);
    if ~mod(step, 50)
        disp([num2str(step), '  loss: ', num2str(lossNew),...
            '  delta: ', num2str(abs(lossNew-lossLast)/lossNew)]);
    end
    temp = find(bound==step);
    if any(temp)
        lr = lrs(temp);
    end
    step = step+1;
    train_loss(step) = lossNew;
end
s = size(train,1);
mse = 1/s * norm(W.*(M-U*V'), 'fro').^2;
disp(['train  ',num2str(mse)]);
%% test
M = zeros(943, 1682);
W = zeros(943, 1682);
M(sub2ind(size(M), test(:,1), test(:,2))) = test(:,3);
W(sub2ind(size(W), test(:,1), test(:,2))) = 1;
s = size(test,1);
mse = 1/s * norm(W.*(M-X), 'fro').^2;
disp(['test   ',num2str(mse)]);
%% save
X = U*V';
save X.mat X;
plot(train_loss(1:step),'linewidth',3);
title('loss');
set(gca,'fontsize',16);