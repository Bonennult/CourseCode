function p = hmm(PI,A,B,O)
% hidden markov model
% input:
%   PI : ��ʼ���ʷֲ� N*1
%   A  : ת�Ƹ��ʾ��� N*N
%   B  : �۲���ʾ��� N*M
%   O  : �۲�����     1*K
%   N �ֿ��ܵ�״̬
%   M �п��ܵĹ۲�ֵ
%   K ���۲�ֵ
% output:
%   p  : �۲���� P(O | A,B,PI) 1*1
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