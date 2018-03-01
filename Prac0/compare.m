for n= [100 1000 10000 100000 1000000]
  x = rand(1,n); 
  y = rand(1,n);

  tic(); 
  mycorr(x,y); 
  t_1 = toc();
  disp(strcat("mycorr took: ", num2str(t_1*1000), " ms to run"));

  tic(); 
  corr(x,y);
  t_2 = toc();
  disp(strcat("corr took: ", num2str(t_2*1000), " ms to run"));
  disp("");
endfor

