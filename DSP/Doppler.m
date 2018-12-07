clear all;  close all;  clc;
[audio, fs] = audioread('sound.wav');
v = 340;    % 声音传播速度
v0 = 0;     % 麦克风速度
vs = 0;     % 
%% STFT
win_width = 400;
overlap = win_width / 2; 
window = ones(win_width,1);
[S, F, T, P] = spectrogram(audio, window, overlap, win_width);
% F = F / (2*pi) * fs;
% surf(T,F,10*log10(P),'edgecolor','none');
% axis tight;
% view(0,90);
% set(gca, 'fontsize', 16);
% xlabel('Time / Seconds'); ylabel('Frequency / Hz');
%figure;
% hold on;
%window = ones(win_width,1);
% subplot(1,2,1);
%spectrogram(audio, window, overlap, win_width)
%set(gca,'fontsize',12);
%title(['Rectangle window with ',num2str(win_width),' samples', num2str(overlap)]);
% window = hamming(win_width);
% subplot(1,2,2);
% spectrogram(audio, window, overlap, win_width)
% set(gca,'fontsize',12);
% title(['Hamming window with ',num2str(win_width),' samples']);
%% 频谱分析
[~, f] = max(abs(S));                  % 每个时间段内最大频率分量
f = f / size(S,1) / 2 * fs;            % 归一化频率转换为实际频率
[pks_max, locs_max] = findpeaks(f);    % 极大值
[pks_min, locs_min] = findpeaks(-f);   % 极小值
f_max = mean(f(locs_max));             % 最大接收频率
f_min = mean(f(locs_min));             % 最小接收频率
c = f_max / f_min;
v_source = v * (c-1) / (c+1);          % 扬声器运动速率
f0 = f_max * (v - v_source) / v;       % 发射频率
period = mean([mean(locs_max(2:end)-locs_max(1:end-1)), mean(locs_min(2:end)-locs_min(1:end-1))]);
r = (win_width - overlap) * period * v_source / (2*pi) / fs;       % 运动半径
len = min(length(locs_max), length(locs_min));
interval = mean(abs(locs_max(1:len) - locs_min(1:len)));    % 最大频率与最小频率出现的间隔
interval = min(interval, period-interval);
theta = interval / period * pi;
d = r / cos(theta);              % 轨迹圆心到麦克风的距离d

display(f0);
display(v_source);
display(r);
display(d);
%% 画图部分
% win_width = [200,400,500,600,800,1000];
% figure;
% hold on;
% for k = 1:length(win_width)
%     % window = hamming(win_width(k));
%     window = ones(win_width(k),1);
%     subplot(2,3,k);
%     spectrogram(audio, window, win_width(k)/2, win_width(k))
%     set(gca, 'fontsize', 12);
%     title([num2str(win_width(k)), ' samples']);
% end
