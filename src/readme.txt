To compile this project, I used g++ with mutiple compilation: all 3 of the cpp files, main.cpp, graph.cpp, and alignment.cpp were compiled together in the form of
> g++ -std=c++11 -pthread -o3 .../main.cpp .../graph.cpp .../alignment.cpp -o .../OUTPUT.out
the tags are important since they specify that c++ 11 is used, and pthreads are used, which helps with the multithreading.  The -o3 optimization is to speed it up since it is a slow program.
This program is fairly slow; I was somewhat disappointed with the runtime.  It took over an hour to run on the full-sized data segment of I tested of 10 clusters of size 100 A large portion of the cost comes from the alignments, which I do all together at the start of the program and store for later use.  I decided to cut this cost by over 1/2, since the alignment of x and y = the alignment of y and x, and all the "central" alignments result in 1.  My initial tests are larger data segments were so atrociously slow I decided this change would be wortht the implementation's complexity.  I assume my method of alignment could also be sped up, but a slow runtime for aligning 1,000 to 10,000 sequences of length ~1000 each will likely always be a slow operation. In current form, this is an O(n^2) operation, since the alignment must be completed for each pair of sequences.
Specific runtime expectations:
Initially, I tested on a single cluster of size 100, (template length 1000, mutation rate of 0.15).  This took about 18 minutes after the revision to enscore so that is only ran the alignment about (n^2)/2 times instead of n^2 times.  The program was printing the probabilities of each grid entry during this running, for debugging purposes, so that should slow the program to some extent.
On the "real" clusters.txt file, with 10 sets of these clusters, the runtime would be expected to be 100 times longer, which would obviously be unacceptable.  
As far as results go, the program seemed fairly effective to me.  I tested a few different theta values from 0.66 to 0.9, and found generally around 0.75 gave the best results for my program.  Note that the alignment score is ranged from 0 to 1, with 0 being no matches and 1 being perfect matches.  sequences with 50% alignment will actually be given a score of 0.666... since the penalty for mismatches is greater than the reward.


Theta comparisons:
First, I tried some small size comparisons of just 2 clusters of 10
theta: 0.75		CAST cluster 1	CAST cluster 2	CAST cluster 3
real cluster 1		90.9091		9.09091		0		
real cluster 2		0		0		100

theta: 0.85		CAST cluster 1	CAST cluster 2	CAST cluster 3 	CAST cluster 4 	CAST cluster 5
real cluster 1		81.8181		9.09091		9.09091		0		0
real cluster 2		0		0		0		90.9091		9.9091

obviously, the tighter theta did "drop" the single elements more than the 1st one did.  For the most part, I found that any lower than 0.75 would tend to combine the clusters too much, whereas higher values tended to treat outliers as their own, "loose" clusters.

I also ran the program on some larger data: I did not have time before the deadline to finish running it on a fullsized data sample, but intend to on my own to compare the multithreaded time to the normal time.  Below are my recordings from the included "clusters.txt" file with theta values as recorded.

theta: 0.85		CAST cluster 1	CAST cluster 2	CAST cluster 3	CAST cluster 4	CAST cluster 5
real cluster 1		96		4		0		0		0
real cluster 2		0		0		90		8		2

Again, we see Cluster 1 contains 96% of the real cluster, and Cluster 3 contains 90% of its real cluster, with a smattering of noise offsetting it.

theta: 0.75			CAST cluster 1	CAST cluster 2	CAST cluster 3	CAST cluster 4	CAST cluster 5	
real cluster 1		96		4		0		0		0		
real cluster 2		0		0		90		8		2		

