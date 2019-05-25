function [P,Z,D] = mypca(X)
% X ÿһ����һ������
    mu = mean(X);
    Y = X - ones(size(X,1),1) * mu;
    Sigma = Y' * Y / size(X,1);
    [P,D] = eig(Sigma);         % �Խǻ�,����ֵ��С��������
    P = rot90(P)';              % ����Գ���������
    D = diag(rot90(rot90(D)));  % ��������ֵ
    Z = X * P;
end