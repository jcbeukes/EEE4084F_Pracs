%Correlation function
function r = mycorr(X,Y)
  N = length(X); 
  Num = dot(X,Y) - sum(X)*sum(Y)/N;
  Den = sqrt((dot(X,X) - (sum(X).^2)/N)*(dot(Y,Y) - (sum(Y).^2)/N));
  r = Num/Den;
endfunction