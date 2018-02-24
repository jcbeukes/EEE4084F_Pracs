% Compare signals shifted in time.
n = [1:0.1:2000];
y = sin(n);
y1 = y(1:100);
y2 = y(11:110);
subplot(2,1,1);
plot(y1);
subplot(2,1,2);
plot(y2);
corr(y1,y2)