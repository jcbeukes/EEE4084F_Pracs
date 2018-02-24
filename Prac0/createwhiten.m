%Custom white noise generator
function white = createwhiten(N)
	white = zeros(length(N*48000)); %Create array
	for i = 1:N*48000+1             %Populate array with random values
		white(i) = rand()*2-1;
	endfor
 endfunction
 