for n= [100 10000 100000 1000000 100000000]
  x = rand(1,n); 
  y = rand(1,n);
  disp(num2str(n));
  myavg = 0;
  avg =0;
  for i = 1:5
    tic(); 
    mycorr(x,y); 
    t_1 = toc();
    myavg = myavg+t_1;
    
    tic(); 
    corr(x,y);
    t_2 = toc();
    avg = avg+t_2;

  endfor
  disp(strcat("mycorr took: ", num2str((myavg/4)*1000), " ms to run"));
  disp(strcat("corr took: ", num2str((avg/4)*1000), " ms to run"));
  disp("");
  
endfor

