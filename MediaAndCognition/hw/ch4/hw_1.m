%% Harr WT
% harr小波分析矩阵
clear all; close all; clc;
H0 = 1;
I0 = eye(1);
for N = 1:3
    shape = 2.^(N-1);
    L = zeros(shape,shape*2);
    B = zeros(shape,shape*2);
    for i = 1:size(L,1)
        L(i,2*(i-1)+1:2*i) = 1;
        B(i,2*(i-1)+1) = 1;
        B(i,2*i) = -1;
    end
    A = 1/sqrt(2)*[H0,zeros(size(H0,1),size(I0,2)); zeros(size(I0,2),size(H0,1)),I0];
    H1 = A * [L; B]; % H1 = sym(A * [L; B]); % 
    H0 = H1;
    I0 = eye(shape*2);
    disp(['----------- N = ',num2str(N),' ----------']);
    disp(L);    disp(B);    disp(H1);    disp(inv(H1));
end

v = [7,11,9,6,4,3,1,3]';
w = H1 * v;
disp(w');

%% PCA and LDA
clear all; close all; clc;
X1 = [1, 2, -5; 5, 9, -3];
X2 = [2, -1, 0; -3, -4, 2];
% PCA
X = [X1,X2];
m = size(X,2);
Xmean = mean(X,2)*ones(1,size(X,2));
S = 1/m * ((X-Xmean) * (X-Xmean)');     % convariance matrix
[P,D] = eig(S);                         % 对角化
[idx,~] = find(D == max(max(D)));       % 寻找最大特征值对应的index
Y = P(:,idx) * (P(:,idx)' * X);  % project
disp('--------- PCA ---------');
disp(mean(X,2)); disp(S); disp(P); disp(D);
figure; hold on;
title('PCA');
plot(X(1,:),X(2,:),'o','MarkerFaceColor','r');
plot(Y(1,:),Y(2,:),'d','MarkerFaceColor','b');
plot(Y(1,:),Y(2,:));
axis equal;

% LDA
mu1 = mean(X1,2)*ones(1,size(X1,2));
mu2 = mean(X2,2)*ones(1,size(X2,2));
Sw = (X1-mu1)*(X1-mu1)' + (X2-mu2)*(X2-mu2)';             % 类内散度
Sb = (mean(X1,2)-mean(X2,2)) * (mean(X1,2)-mean(X2,2))';  % 类间散度
S = Sw\Sb;
[P,D] = eig(S);                         % 对角化
[idx,~] = find(D == max(max(D)));       % 寻找最大特征值对应的index
Y1 = P(:,idx) * (P(:,idx)' * X1);       % project
Y2 = P(:,idx) * (P(:,idx)' * X2);       % project
disp('--------- LDA ---------');
disp(mu1); disp(mu2); disp(Sw); disp(Sb);
disp(S); disp(P); disp(D);
figure; hold on;
title('LDA');
plot(X1(1,:),X1(2,:),'ro','MarkerFaceColor','r');
plot(X2(1,:),X2(2,:),'bo','MarkerFaceColor','b');
plot(Y1(1,:),Y1(2,:),'^','MarkerFaceColor','r');
plot(Y2(1,:),Y2(2,:),'v','MarkerFaceColor','b');
plot([Y1(1,:),Y2(1,:)],[Y1(2,:),Y2(2,:)]);
axis equal;

%% Gabor filters
clear all; close all; clc;
load('ORL_64x64.mat');
im = reshape(fea(1,:),[64,64]);
% imshow(im);
% image(im);
% kxx = [0.1,1,10];
% kyy = [0.1,1,10];
kxx = [0,1,1];
kyy = [1,0,1];
sigma = [0.5,2,5];

figure(1);
for i = 1:3
    sigm = sigma(2);
    kx = kxx(i);
    ky = kyy(i);
    [x,y] = meshgrid(-round(3*sigm):round(3*sigm));
    G_even = cos(kx*x + ky*y) * exp(-(x.^2+y.^2)/2/sigm.^2);
    G_odd = sin(kx*x + ky*y) * exp(-(x.^2+y.^2)/2/sigm.^2);
    fe = filter2(G_even, im);
    fo = filter2(G_odd, im);

    subplot(3,3,3*(i-1)+1); imshow(im, []); title(['kx=',num2str(kx)]);
    subplot(3,3,3*(i-1)+2); imshow(fe, []); title(['ky=',num2str(ky)]);
    subplot(3,3,3*(i-1)+3); imshow(fo, []); title(['\sigma=',num2str(sigm)]);
end
figure(2);
imshow([im fe fo], []);

[U,Z,lambda] = princomp(fea);    % pca变换
%[U,Z,lambda] = mypca(fea);
eigface = Z(:,1:10)*U(:,1:10)';

figure(2);
for i = 1:10
    eigface = Z(1,i)*U(:,i)';
    im = reshape(eigface,[64,64]);
    subplot(2,5,i); imshow(im, []);
end

figure(3);
N = [10,100,500,1000];
for i=1:length(N)
%     disp(mean(fea(1,:)));
    eigface = Z(1,1:N(i))*U(:,1:N(i))'; % + mean(fea(1,:));
    face = reshape(fea(1,:),[64,64]);
    err = fea(1,:) - eigface;
    im = reshape(eigface,[64,64]);
    err = reshape(err,[64,64]);

    subplot(length(N),3,3*(i-1)+1); imshow(face, []); title('initial');
    subplot(length(N),3,3*(i-1)+2); imshow(im, []); title(['N=',num2str(N(i))]);
    subplot(length(N),3,3*(i-1)+3); imshow(err, []); title('error');
end