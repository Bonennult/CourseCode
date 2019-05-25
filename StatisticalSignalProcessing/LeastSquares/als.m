% 基于 ALS 的推荐算法
clear all;  close all; clc;
load data_train.mat;
%% hyperparameters
ranks = [10,30,100];
lambdas = [0.1,1,10,100,500,1000];       % 不同的lambda差不多，100的效果稍差
epsilon = 1e-7;%0.00001;        % limit of iteration
%% randomly select train data and test data
%U = randn(943, rank);
%V = randn(1682, rank);
idx = randperm(90000,80000);
train = data_train(idx,:);
temp = 1:90000;
temp(idx) = 0;
temp(temp==0) = [];
test = data_train(temp,:);
% for train
M = zeros(943, 1682);
W = zeros(943, 1682);
M(sub2ind(size(M), train(:,1), train(:,2))) = train(:,3);
W(sub2ind(size(W), train(:,1), train(:,2))) = 1;
W0 = W;
W0(sub2ind(size(W0), test(:,1), test(:,2))) = 1;   % all data
% for test
Mtest = zeros(943, 1682);
Wtest = zeros(943, 1682);
Mtest(sub2ind(size(M), test(:,1), test(:,2))) = test(:,3);
Wtest(sub2ind(size(W), test(:,1), test(:,2))) = 1;
s = size(test,1);
bestmse=1e10;
%% train
for i=1:length(ranks)
    rank = ranks(i);
    lambda = 10;%lambdas(i);
    step = 0;
    U = randn(943, rank);
    V = randn(1682, rank);
    I = eye(rank);
    lossLast = 0;
    lossNew = norm(W.*(M-U*V'), 'fro').^2 + lambda/2*(norm(U,'fro').^2 + norm(V,'fro').^2);
    disp(lossNew);
    while step<=1000 && abs(lossLast-lossNew)/lossNew>epsilon
        lossLast=lossNew;
        lastU = U;
        U = M*V/(V'*V+lambda*I);
        lastV = V;
        V = M'*U/(U'*U+lambda*I);
        lossNew = norm(W.*(M-U*V'), 'fro').^2 + lambda/2*(norm(U,'fro').^2 + norm(V,'fro').^2);
        diffUmax = max(max(abs(U-lastU)));     % 两次迭代之间U，V值变化的平均和最大值
        diffUmean = mean(mean(abs(U-lastU)));
        diffVmax = max(max(abs(V-lastV)));
        diffVmean = mean(mean(abs(V-lastV)));
        diff = W0.*(M-U*V');
        diffmax = max(max(abs(diff)));
        diffmean = mean(mean(abs(diff(W0~=0))));
        if ~mod(step, 500)
            disp([num2str(step), '  loss: ', num2str(lossNew),...
                '  delta: ', num2str(abs(lossNew-lossLast)/lossNew),...
                '  diffmax: ',num2str(diffmax),...
                '  diffmean: ',num2str(diffmean),...
                '  diffUmax: ',num2str(diffUmax),...
                '  diffUmean: ',num2str(diffUmean),...
                '  diffVmean: ',num2str(diffVmean),...
                ]);

            testmean = mean(mean(Mtest(Wtest~=0)));    % mean value of test data
            difftest = Wtest.*(Mtest-U*V');
            difftestmax = max(max(abs(difftest)));     % 计算得到的 UV 在测试数据上误差的最大值和平均值
            difftestmean = mean(mean(abs(difftest(Wtest~=0))));
            mse = 1/s * norm(Wtest.*(Mtest-U*V'), 'fro').^2;
            disp(['     test   ',num2str(mse),...
                '  mean: ',num2str(testmean),...
                '  difftestmax: ',num2str(difftestmax),...
                '  difftestmean: ',num2str(difftestmean)]);
            disp(' ');
        end
        step = step+1;
    end
    % train mse
    s = size(train,1);
    mse = 1/s * norm(W.*(M-U*V'), 'fro').^2;
    disp(['train   ',num2str(mse)]);
    % test
    Mtest = zeros(943, 1682);
    Wtest = zeros(943, 1682);
    Mtest(sub2ind(size(M), test(:,1), test(:,2))) = test(:,3);
    Wtest(sub2ind(size(W), test(:,1), test(:,2))) = 1;
    s = size(test,1);
    mse = 1/s * norm(Wtest.*(Mtest-U*V'), 'fro').^2;
    if mse < bestmse
        bestmse = mse;
        X = U*V';
        save X_als.mat X;
    end
    disp(['test   ',num2str(mse),...
        '   step',num2str(step)]);
end