tic; createwhiten(10); runtime = toc();
disp(strcat("It took: ", num2str(runtime*1000), " ms to run"));

tic; createwhiten(20); runtime = toc();
disp(strcat("It took: ", num2str(runtime*1000), " ms to run"));

tic; createwhiten(50); runtime = toc();
disp(strcat("It took: ", num2str(runtime*1000), " ms to run"));

tic; createwhiten(100); runtime = toc();
disp(strcat("It took: ", num2str(runtime*1000), " ms to run"));