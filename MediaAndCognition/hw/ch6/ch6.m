clear all; close all; clc;
%% LDA
X1 = [1 2 2; ...
    1 2 0];
X2 = [0  1  0.5; ...
    0 0.5  1];
mu1 = mean(X1,2)*ones(1,size(X1,2));
mu2 = mean(X2,2)*ones(1,size(X2,2));
Sw = (X1-mu1)*(X1-mu1)' + (X2-mu2)*(X2-mu2)';             % ����ɢ��
Sb = (mean(X1,2)-mean(X2,2)) * (mean(X1,2)-mean(X2,2))';  % ���ɢ��
S = Sw\Sb;
[P,D] = eig(S);                         % �Խǻ�
[idx,~] = find(D == max(max(D)));       % Ѱ���������ֵ��Ӧ��index
Y1 = P(:,idx) * (P(:,idx)' * X1);       % project
Y2 = P(:,idx) * (P(:,idx)' * X2);       % project
w = Sw\(mean(X1,2)-mean(X2,2));         % Fisher ͶӰ����
w0 = w' * (mu1(:,1)+mu2(:,2))/2;        % ������ֵ
x2 = [X1(2,:), X2(2,:)];
x1 = (w0 - w(2)*x2) / w(1);             % �������
disp('--------- LDA ---------');
disp(mu1); disp(mu2); disp(Sw); disp(Sb);
disp(S); disp(P); disp(D);
figure; hold on;
title('LDA');
plot(X1(1,:),X1(2,:),'ro','MarkerFaceColor','r');    % ���1������
plot(X2(1,:),X2(2,:),'bo','MarkerFaceColor','b');    % ���2������
plot(Y1(1,:),Y1(2,:),'^','MarkerFaceColor','r');     % ���1ͶӰ��
plot(Y2(1,:),Y2(2,:),'v','MarkerFaceColor','b');     % ���2ͶӰ��
plot([Y1(1,:),Y2(1,:)],[Y1(2,:),Y2(2,:)]);           % ͶӰ����
plot(x1,x2);                                         % �������
axis equal;
%% svm
X1 = [1 2 2; ...
    1 0 2];
X2 = [0 1 0; ...
    0 0 1];
w = [2,2]';          % ���ż��Ȩ����
b = -3;
x2 = [X1(2,:),X2(2,:)];
x1 = (-b - w(2)*x2) / w(1);    % ���ų�ƽ��
figure; hold on;
title('SVM');
plot(X1(1,:),X1(2,:),'ro','MarkerFaceColor','r');    % ���1������
plot(X2(1,:),X2(2,:),'bo','MarkerFaceColor','b');    % ���2������
plot(x1,x2);   % ���ų�ƽ��
%% svm image classification
% load data
load Caltech-256_VGG_10classes.mat
x_test = traindata.X;
y_test = traindata.y;
x_train = testdata.X;
y_train = testdata.y;
train_accr = zeros(1,10);
test_accr = zeros(1,10);
%����SVMѵ�������ò���������svmtrain/svmclassify��
X = 1:10;
C = [0.01 0.1 1 5];
kernel = {'linear', 'polynomial', 'rbf', 'mlp'};
for j = 1:length(kernel)
    figure;
    set(gcf,'outerposition',get(0,'screensize'));
    hold on;
    for i = 1:length(C)
        disp(['------------- C = ',num2str(C(i)),' | kernel : ',char(kernel(j)),' -------------']);
        for k = 1 : 10
            model = svmtrain(x_train', (y_train == k)',...
                'boxconstraint', C(i),...
                'kernel_function', char(kernel(j)));     %�˴����޸�
            y_pred = svmclassify(model, x_train'); %?�˴����޸�
            label = (y_train == k);
            train_accr(k) = length(find(y_pred' == label))/length(label);
            fprintf( 'train accuracy for class %d is: %f\n',...
                k, train_accr(k));
            
            y_pred = svmclassify(model, x_test');
            label = (y_test == k);
            test_accr(k) = length(find(y_pred' == label))/length(label);
            fprintf( 'test  accuracy for class %d is: %f\n',...
                k, test_accr(k));
        end
        subplot(2,2,i);
        hold on;
        title(['Accuracy | C = ', num2str(C(i)),' | kernel : ',char(kernel(j))]);
        stem(X,train_accr,'b','filled','linewidth',2);
        stem(X+0.3,test_accr,'r','filled','linewidth',2);
        legend('train','test');
        %axis tight;
        axis([1 10.3 0 1]);
        saveas(gcf,['accur_',char(kernel(j)),'_2.png']);
    end
end