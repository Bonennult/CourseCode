clear all; close all; clc;
load Dictionary.mat
imgnum = 1;
img = imread([num2str(imgnum),'_lr.bmp']);
% 如果长宽不能被3整除，边界补0
overlap = 1;
ps = 3;                         % patch size
[h, w, ~] = size(img);
W = ceil((w-ps)/(ps-overlap))+1;
H = ceil((h-ps)/(ps-overlap))+1;
img2proc = uint8(zeros((H-1)*(ps-overlap)+ps, (W-1)*(ps-overlap)+ps, 3));   % 一定要转化成uint8 ！！！
img2proc(1:h, 1:w, :) = uint8(img);
% RGB空间变换到YCbCr空间
scale = 3;                       % 放大因子
YCClr = double(rgb2ycbcr(img2proc));
YCCsr = double(rgb2ycbcr(imresize(img2proc, scale)));   % 默认bicubic插值 % 转化成double，防止overlap处溢出
YCCsr(:,:,1) = 0;
flag = zeros(size(YCCsr(:,:,1)));
% 放大两倍，取出亮度分量
Ylr = YCClr(:,:,1);
Ymr = imresize(Ylr, 2, 'bicubic');
% 计算特征
FYmr(:,:,1) = imfilter(Ymr, [-1,0,1], 'replicate', 'conv');
FYmr(:,:,2) = imfilter(Ymr, [-1,0,1]', 'replicate', 'conv');
FYmr(:,:,3) = imfilter(Ymr, [1,0,-2,0,1], 'replicate', 'conv');
FYmr(:,:,4) = imfilter(Ymr, [1,0,-2,0,1]', 'replicate', 'conv');
% Yfeatures(:,:,1) = conv2(Ymr, [-1,0,1], 'same');        % 边界自动补0
% Yfeatures(:,:,2) = conv2(Ymr, [-1,0,1]', 'same');
% Yfeatures(:,:,3) = conv2(Ymr, [1,0,-2,0,1], 'same');
% Yfeatures(:,:,4) = conv2(Ymr, [1,0,-2,0,1]', 'same');
% 计算稀疏表达
%lambda = 50*4*ps*ps;
lambda = 0.001;
numalpha = zeros(H*W,1);
for m = 1:H
    for n = 1:W
        startR = (m-1)*(ps-overlap);
        startC = (n-1)*(ps-overlap);
        patch = Ymr(startR*2+1:startR*2+ps*2, startC*2+1:startC*2+ps*2, :);
        patchmean = mean(patch(:));
        y = FYmr(startR*2+1:startR*2+ps*2, startC*2+1:startC*2+ps*2, :);
        y = y(:);
        normalization_m = sqrt(sum(y.^2));
        if normalization_m > 1,
            yy = y./normalization_m;
        else
            yy = y;
        end
        % 计算稀疏表达
        %alpha = optimize(Dl, y(:), lambda);
        alpha = lasso(Dl, yy, 'Lambda', lambda); % 'DFmax', 35,
        numalpha((m-1)*H+n) = sum(alpha~=0);
        % 重建高分辨率图像块
        % x = Dh * alpha;
        if normalization_m > 1,
            x = Dh * alpha * normalization_m;
        else
            x = Dh * alpha;
        end
        YCCsr(startR*scale+1:startR*scale+ps*scale,...
            startC*scale+1:startC*scale+ps*scale, 1) = ...
            YCCsr(startR*scale+1:startR*scale+ps*scale, ...
            startC*scale+1:startC*scale+ps*scale, 1) + ...
            double(reshape(x,[ps*scale,ps*scale])+patchmean);
        flag(startR*scale+1:startR*scale+ps*scale,...
            startC*scale+1:startC*scale+ps*scale) = ...
            flag(startR*scale+1:startR*scale+ps*scale,...
            startC*scale+1:startC*scale+ps*scale) + 1;
    end
end
YCCsr(:,:,1) = YCCsr(:,:,1)./flag;
img_sr = ycbcr2rgb(uint8(YCCsr));
img_sr = img_sr(1:h*scale, 1:w*scale, :);
% save([num2str(imgnum),'_sr_overlap.mat'], 'img_sr', 'lambda', 'numalpha', 'overlap');
figure;
hold on;
set(gca, 'fontsize', 16);
a1 = subplot(2,2,1);
imshow(img);
title('Original Low Resolution');
a2 = subplot(2,2,2);
imshow(imread([num2str(imgnum),'.bmp']));
title('Original High Resolution');
a3 = subplot(2,2,3);
imshow(imresize(img, 3));
title('Bicubic Iterpolation');
a4 = subplot(2,2,4);
imshow(img_sr);
title('Super Resolution as Sparse Representation with Overlap');
% 原始分辨率显示
xsize = get(a2, 'XLim');
ysize = get(a2, 'YLim');
set(a1, 'XLim',xsize,'YLim',ysize);
set(a3, 'XLim',xsize,'YLim',ysize);
set(a4, 'XLim',xsize,'YLim',ysize);

figure;
plot(numalpha, 'linewidth', 2);
title(['Alpha with lambda=', num2str(lambda)]);