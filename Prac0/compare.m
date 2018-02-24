x = rand(100,1); 
y = rand(100,1);

tic; mycorr(x,y); t1 = toc()
tic; corr(x,y); t2 = toc()

x = rand(1000,1);
y = rand(1000,1);

tic; mycorr(x,y); t1 = toc()
tic; corr(x,y); t2 = toc()

x = rand(10000,1); 
y = rand(10000,1); 

tic; mycorr(x,y); t1 = toc()
tic; corr(x,y); t2 = toc()

%avg_speedup = (speedup_1+speedup_2+speedup_3)/3