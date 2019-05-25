clear all; close all; clc;
%% test myhmm
A = [0.7 0.3; 0.2 0.8];
B = [0.4 0.1 0.4 0.1; 0.2 0.3 0.2 0.3];
PI = [0.4 0.6];
O = [2 1 4];
p = hmm(PI, A, B, O);
disp(p);

A = {'ABCD','BCDEFG','ABD'};
for i = 1:length(A)
    num2str(uint8(char(A(i))-'A'));
end

%% HMM
clear all; clc;
seq1 = { double('AABBCCDD'), double('ABBCBBDD'), double('ACBCBCD'), ...
        double('AD'), double('ACBCBABCDD'), double('BABAADDD'), ...
        double('BABCDCC'), double('ABDBBCCDD'), double('ABAAACDCCD'), double('ABD')}; %��������
seq2 = { double('DDCCBBAA'), double('DDABCBA'), double('CDCDCBABA'), ...
        double('DDBBA'), double('DADACBBAA'), double('CDDCCBA'), ...
        double('BDDBCAAAA'), double('BBABBDDDCD'), double('DDADDBCAA'), double('DDCAA')}; %��������
A0 = unifrnd(0,1,3,3);   % ��ʼ��״̬ת�Ƹ��ʾ���
B0 = unifrnd(0,1,3,4);   % ��ʼ���۲���ʾ���
A0 = A0 ./ (sum(A0,2)*ones(1,size(A0,2)));
B0 = B0 ./ (sum(B0,2)*ones(1,size(B0,2)));
% ����hmmtrain��������״̬ת�Ƹ����Լ��۲����
[Ae1, Be1] = hmmtrain(seq1, A0, B0, 'Symbols', double('ABCD'));
% ����hmmdecode���ز������г��ֶ�����Ȼ�ȣ����ָ��ʵĶ�����
[ ~, log_prob_seq ] = hmmdecode(double('ABBBCDDD'), Ae1, Be1, 'Symbols', double('ABCD'));
% ������������ɸ�HMM�����ĸ���
prob_seq = exp( log_prob_seq );
disp(prob_seq);

[ ~, log_prob_seq ] = hmmdecode(double('DADBCBAA'), Ae1, Be1, 'Symbols', double('ABCD'));
prob_seq = exp( log_prob_seq );
disp(prob_seq);

[ ~, log_prob_seq ] = hmmdecode(double('CDCBABA'), Ae1, Be1, 'Symbols', double('ABCD'));
prob_seq = exp( log_prob_seq );
disp(prob_seq);

[ ~, log_prob_seq ] = hmmdecode(double('ADBBBCD'), Ae1, Be1, 'Symbols', double('ABCD'));
prob_seq = exp( log_prob_seq );
disp(prob_seq);

disp('----------------------------')
% ����hmmtrain��������״̬ת�Ƹ����Լ��۲����
[Ae2, Be2] = hmmtrain(seq2, A0, B0, 'Symbols', double('ABCD'));
% ����hmmdecode���ز������г��ֶ�����Ȼ�ȣ����ָ��ʵĶ�����
[ ~, log_prob_seq ] = hmmdecode(double('ABBBCDDD'), Ae2, Be2, 'Symbols', double('ABCD'));
% ������������ɸ�HMM�����ĸ���
prob_seq = exp( log_prob_seq );
disp(prob_seq);

[ ~, log_prob_seq ] = hmmdecode(double('DADBCBAA'), Ae2, Be2, 'Symbols', double('ABCD'));
prob_seq = exp( log_prob_seq );
disp(prob_seq);

[ ~, log_prob_seq ] = hmmdecode(double('CDCBABA'), Ae2, Be2, 'Symbols', double('ABCD'));
prob_seq = exp( log_prob_seq );
disp(prob_seq);

[ ~, log_prob_seq ] = hmmdecode(double('ADBBBCD'), Ae2, Be2, 'Symbols', double('ABCD'));
prob_seq = exp( log_prob_seq );
disp(prob_seq);
%% 
tr = [0.95,0.05;
      0.10,0.90];

e = [1/6,  1/6,  1/6,  1/6,  1/6,  1/6;
     1/10, 1/10, 1/10, 1/10, 1/10, 1/2;];

seq1 = hmmgenerate(100,tr,e);
seq2 = hmmgenerate(200,tr,e);
seqs = {seq1,seq2};
[estTR, estE] = hmmtrain(seqs,tr,e);