% Compare signals shifted in time.
x = -20:0.1:20;
outp = [];
h = 1;
for n = 0:0.1:2*pi
  y1 = sin (x);
  y2 = sin(x+n);
  outp(h) = corr(y1, y2);
  h = h + 1;
endfor
n = 0:0.1:2*pi;
plot(n, outp);
axis([0 2*pi -1 1]);
xlabel('Phase shift');
ylabel('Correlation');    
title('Correlation between sinusoids as a function of phase shift');
 set(gca,'fontsize',20); 