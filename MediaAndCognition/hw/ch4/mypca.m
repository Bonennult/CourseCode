function [P,Z,D] = mypca(X)
% X 每一行是一组数据
    mu = mean(X);
    Y = X - ones(size(X,1),1) * mu;
    Sigma = Y' * Y / size(X,1);
    [P,D] = eig(Sigma);         % 对角化,特征值从小到大排列
    P = rot90(P)';              % 镜像对称特征向量
    D = diag(rot90(rot90(D)));  % 重排特征值
    Z = X * P;
end