function p = hmm(PI,A,B,O)
% hidden markov model
% input:
%   PI : 初始概率分布 N*1
%   A  : 转移概率矩阵 N*N
%   B  : 观测概率矩阵 N*M
%   O  : 观测序列     1*K
%   N 种可能的状态
%   M 中可能的观测值
%   K 个观测值
% output:
%   p  : 观测概率 P(O | A,B,PI) 1*1
assert(size(A,1)==size(A,2),['A is not square matrix with shape (',num2str(size(A)),')']);
assert(size(PI,2)==size(A,1),['PI cannot match A with shape (',num2str(size(PI)),')']);
assert(size(B,1)==size(A,1),['B cannot match A with shape (',num2str(size(B)),')']);
assert(all(O>=1) && all(O<=size(B,2)), ['Wrong sequence O']);
for i = 1:size(O,1)
    for j = 1:size(O,2)
        p = PI * diag(B(:,O(i,j)));
        PI = p * A;
    end
end
% disp(p);
p = sum(p,2);
end