clear all; close all;  clc;
%% predefine
h = 6.63e-34;   % plank constant
m0 = 9.1e-31;   % mass of electronic
a = 5.43e-10;   % lattice constant
x = linspace(-a/2, a/2, 101);
V = x>=-a/4 & x<=a/4;
V = V .* cos(x*2*pi/a) * 1e-19;  % potential field
% plot(x,V);
k = linspace(-pi/a * 1.2 , pi/a * 1.2 , 97);
%% compute enerfy band
N = length(V);
fs = fft(V) / N;          % Fourier Series of peoriodic potential
disp('Fourier series');
disp(abs(fs(2:4))*2);
v = flip([fs, fs]);        % v is Conjugate symmetric
L = 9;                   % get L eigen roots
Hamilton = zeros(L, L);
t = -floor(L/2):floor(L/2);
for m = 1:L
    Hamilton(m,:) = v(N+ceil(L/2)-m+t);
end
t = t * 2*pi / a;
t = t' * ones(1,length(k));
K = ones(L,1) * k;
K = (K + t).^2 * h.^2 / (8 * pi.^2 * m0);
figure;
hold on;
Ne = 4;         % num of band
E = zeros(length(k),Ne);
gap = zeros(1,3);
for s = 1:length(k)
    H = Hamilton + diag(K(:,s));
    e = eig(H);
    e = sort(e);
    E(s,:) = e(1:Ne);
    temp = k(s)*ones(1,Ne);
    scatter(temp, e(1:Ne),'ko');
end
gap(1) = E(5,2) - E(5,1);
gap(3) = E(5,4) - E(5,3);
gap(2) = E(25,3) - E(25,2);
disp('Numerical calculation');
disp(gap);
%% compare
k =linspace(-pi/a * 1.2 , pi/a * 1.2 , 97);
tempk = k(1:17);
Ek = h.^2/(8*pi.^2*m0)*k.^2;
e1 = Ek(1:17);
e2 = Ek(81:97);
E1 = (e1+e2 + sqrt((e1-e2).^2 + 4 * abs(fs(2)).^2))/2;
E2 = (e1+e2 - sqrt((e1-e2).^2 + 4 * abs(fs(2)).^2))/2;
Ek = h.^2/(8*pi.^2*m0)*(k-2*pi/a).^2;
e1 = Ek(1:17);
Ek = h.^2/(8*pi.^2*m0)*(k+2*pi/a).^2;
e2 = Ek(81:97);
E3 = (e1+e2 + sqrt((e1-e2).^2 + 4 * abs(fs(3)).^2))/2;
E4 = (e1+e2 - sqrt((e1-e2).^2 + 4 * abs(fs(3)).^2))/2;
figure;
hold on;
scatter(tempk,E(1:17,1),'ko');
scatter(tempk,E(1:17,2),'ko');
scatter(tempk,E(1:17,3),'ko');
scatter(tempk,E(1:17,4),'ko');
plot(tempk,E1);
plot(tempk,E2);
plot(tempk,E3);
plot(tempk,E4);