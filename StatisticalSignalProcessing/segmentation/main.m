%% read img
function main
clear all; close all; clc;
img1 = imread('1.bmp');
img2 = imread('2.bmp');
% remove boat, log is best
method = 'log';
img1g = rgb2gray(img1);
img2g = rgb2gray(img2);
edge1 = edge(img1g, method);
edge2 = edge(img2g, method);
edge3 = edge1 & edge2;
PLOT(img1g,edge1,edge2,edge3,method);
% ��ǿ��Ե���Ա��ڽ��к�½�ָ�
img2seg = edge3;  % edge3 for seg land and sea
win = [11, 21];
stride = [3, 3];
thresh1 = win .* [1/3, 5];       % ��ֵ1
thresh2 = win .* [1/5, 1];       % ��ֵ2
interval = [1 1 size(img2seg,2) size(img2seg,1)];  % ���Ͻ��к��У���͸�
img2seg = ENHANCE(img2seg,win,stride,thresh1,thresh2,interval);
% gaussian smmoth
img2seg = double(img2seg);
win = 21;
f = fspecial('gaussian',[win win],20);    % ��˹ģ��
imgSmooth = imfilter(img2seg, f, 'same');         % �˲�
%figure;
%imshow(imgSmooth);
% ��½�ָ�
thresh = 0.10;
seg = imgSmooth<thresh;
figure;
imshow(1-seg);
segResult = SEG(img1,seg);

% ��ǿ��Ե���Ա�����ȡ����λ��
img2seg = edge1;  % edge1 for seg boats
win = 3;
stride = 1;
thresh1 = win .* 1/3;       % ��ֵ1
thresh2 = win .* 1/5;       % ��ֵ2
interval = [1 1 size(img2seg,2) size(img2seg,1)];  % ���Ͻ��к��У���͸�
img2seg = ENHANCE(img2seg,win,stride,thresh1,thresh2,interval);
% ƥ���⴬��
win = 91;
stride = 3;
thresh1 = win*6;            % ��ֵ1
template = zeros(win,win);
center = int16((win+1)/2);
template(center-22:center+22,center-22:center+22) = 2;  % ���Ĵ�С��ԼΪ45*45���أ�����۲�
template = template-1;
interval = [1 1 size(img2seg,2) size(img2seg,1)];  % ���Ͻ��к��У���͸�
img2seg = MATCH(img2seg,template,win,stride,thresh1,interval);
% ��ȡ�����λ��
thresh1 = win * 5;
seg = img2seg >= thresh1;
stride = 1;
w1 = int16((stride-1)/2);
w2 = int16(22);
i = interval(1)+w2;     % i�У�j��
while i+w1<interval(1)+interval(3)
    j = interval(2)+w2;
    while j+w1<interval(2)+interval(4)
        if img2seg(j,i)
            seg(j-w2:j+w2, i-w2:i+w2) = 1;
        end
        j = j+stride;
    end
    i = i+stride;
end
figure;
imshow(seg);
seg = ~seg;
segResult = SEG(img1,seg);
end
%% function utilized
function tgtimg = ENHANCE(srcimg,winSize,stride,threshUp,threshDown,interval)
% ��srcimg������ǿ����������ͺ����ĶԱȶ�
% input
%   srcimg: ����ǿͼ��
%   winSize: ��������С
%   stride: ��������
%   threshUp: ����ֵ
%   threshDown: ����ֵ
%   interval: ��ǿ����
% output
%   tgtimg: ��ǿ���ͼ��
tgtimg = srcimg;
for k=1:length(winSize)
    seg = tgtimg;
    w1 = int16((winSize(k)-1)/2);
    w2 = int16((stride(k)-1)/2);
    th1 = threshUp(k);
    th2 = threshDown(k);
    i = interval(1)+w1;     % i�У�j��
    while i+w1<interval(1)+interval(3)
        j = interval(2)+w1;
        while j+w1<interval(2)+interval(4)
            t = sum(sum(tgtimg(j-w1:j+w1, i-w1:i+w1)));
            if t>=th1
                seg(j-w2:j+w2,i-w2:i+w2) = 2;
            elseif t<=th2
                seg(j-w2:j+w2,i-w2:i+w2) = 0;
            end
            j = j+stride(k);
        end
        i = i+stride(k);
    end
    %figure;
    %imshow(seg);
    tgtimg = seg;
end
end

function tgtimg = MATCH(srcimg,template,winSize,stride,thresh,interval)
% ����ƥ��ģ��template��srcimg���м��
% input
%   srcimg: �����ͼ��
%   template: ƥ��ģ��
%   winSize: ƥ��ģ�崰��С
%   stride: ��������
%   thresh: �о���ֵ
%   interval: �������
% output
%   tgtimg: �����
tgtimg = srcimg;
for k=1:length(winSize)
    seg = double(tgtimg);
    w1 = int16((winSize(k)-1)/2);
    w2 = int16((stride(k)-1)/2);
    th = thresh(k);
    i = interval(1)+w1;     % i�У�j��
    while i+w1<interval(1)+interval(3)
        j = interval(2)+w1;
        while j+w1<interval(2)+interval(4)
            t = sum(sum(tgtimg(j-w1:j+w1, i-w1:i+w1).*template));
            if t>=th
                if (j<=interval(2)+w1 || j+w1>=interval(2)+interval(4)-stride-1 || i <= interval(1)+w1 || i+w1>=interval(1)+interval(3)-stride-1)
                    seg(j-w1:j+w1,i-w1:i+w1) = 0; % �����������޷������ĸ���Ե��ЧӦ
                end
                seg(j,i) = t;
            else
                seg(j-w2:j+w2,i-w2:i+w2) = 0;
                if (j<=interval(2)+w1 || j+w1>=interval(2)+interval(4)-stride-1 || i <= interval(1)+w1 || i+w1>=interval(1)+interval(3)-stride-1)
                    seg(j-w1:j+w1,i-w1:i+w1) = 0; % �����������޷������ĸ���Ե��ЧӦ
                end
            end
            j = j+stride(k);
        end
        i = i+stride(k);
    end
    %figure;
    %imshow(seg);
    tgtimg = seg;
end
end

function segimg = SEG(srcimg,segidx)
% ����segidx�ָ�srcimgͼ��
% input
%   srcimg: ���ָ�rgbͼ��
%   segidx: ����ָʾ�ָ�Ķ�ֵͼ����С��srcimg��ͬ
% output
%   segimg: �ָ���ͼ��
segimg = srcimg;
temp = srcimg(:,:,1);
temp(segidx) = 0;
segimg(:,:,1) = temp;
temp = srcimg(:,:,2);
temp(segidx) = 0;
segimg(:,:,2) = temp;
temp = srcimg(:,:,3);
temp(segidx) = 0;
segimg(:,:,3) = temp;
figure;
imshow(segimg);
end

function PLOT(img1,img2,img3,img4,method)
figure;
hold on;
subplot(2,2,1);
imshow(img1);
title(['img1 ',method]);
subplot(2,2,2);
imshow(img2);
title('edge1');
subplot(2,2,3);
imshow(img3);
title('edge2');
subplot(2,2,4);
imshow(img4);
title('edge1 and edge2');
end