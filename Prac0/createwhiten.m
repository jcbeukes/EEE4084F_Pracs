%Custom white noise generator
function white = createwhiten(N)
    n = N*48000;

	white = zeros(length(n)); %Create array
  
	for i = 1:n             %Populate array with random values
		white(i) = rand()*2-1;
	endfor
 endfunction
 