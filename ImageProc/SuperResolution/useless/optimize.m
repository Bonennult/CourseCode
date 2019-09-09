function alpha = optimize(D, y, lambda)
% 应用梯度下降法求解 alpha
% min norm(alpha,1) s.t. norm(D*alpha-y,2)^2 < epslion
% min lambda*norm(alpha,1) + 1/2*norm(D*alpha-y,2)^2
lr = 1e-5;
maxstep = 300;
epsilon = 1e-5;
alpha = randn(size(D,2), 1);
lastloss = lambda*norm(alpha,1) + 1/2*norm(D*alpha-y)^2;
gd = lambda*sign(alpha) + D'*(D*alpha-y);
alpha = alpha - lr * gd;
loss = lambda*norm(alpha,1) + 1/2*norm(D*alpha-y)^2;
step = 0;
while step < maxstep && abs(loss-lastloss)/loss > epsilon
    lastloss = loss;
    gd = lambda*sign(alpha) + D'*(D*alpha-y);
    alpha = alpha - lr * gd;
    loss = lambda*norm(alpha,1) + 1/2*norm(D*alpha-y)^2;
    step = step+1;
end