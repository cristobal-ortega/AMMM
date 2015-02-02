#include <iostream>
#include "SampleDecoder.h"
#include "MTRand.h"
#include "BRKGA.h"
#include <sys/time.h>


float penalty;
int n;		//# of structs
int m;		//# of banks
int o;		//# of conflicts

/* C = Capacities 	*/
/* S = sizes of each data structure 	*/
/* E = Access cost of each data struct 	*/
/* D = Conflicts found in the program   */

int *C_orig;
int *S_orig;
int *E_orig;
int *A_orig;
int *B_orig;
int *D_orig;




using namespace std;

void readData(){
	/* read input with ... */
	cin >> penalty;
	/* ... data struct info */
	cin >> n;
	S_orig = (int *) malloc( n*sizeof(int) );
	for(int i = 0; i < n; i++){
		cin >> S_orig[i];
	}
	
	E_orig = (int *) malloc( n*sizeof(int) );
	for(int i = 0; i < n; i++){
		cin >> E_orig[i];
	}

	

	/* memory bank info */
	cin >> m;
	C_orig = (int *) malloc( m*sizeof(int) );	
	for(int j = 0; j < m; j++){
		cin >> C_orig[j];
	}

	
	/* 	conflicts info
		A[0..o-1]
		B[0..o-1]
		D[0..o-1]
	*/
	cin >> o;
	A_orig = (int *) malloc( o*sizeof(int) );
	B_orig = (int *) malloc( o*sizeof(int) );
	D_orig = (int *) malloc( o*sizeof(int) );
	for(int k = 0; k < o; k++)
		cin >> A_orig[k];
	for(int k = 0; k < o; k++){
		cin >> B_orig[k];
	}	
	for(int k = 0; k < o; k++){
		cin >> D_orig[k];
	}	
}

int main(int argc, char* argv[]) {
	//const unsigned n = 8;		// size of chromosomes
	const unsigned p = 100;	// size of population
	const double pe = 0.20;		// fraction of population to be the elite-set
	const double pm = 0.10;		// fraction of population to be replaced by mutants
	const double rhoe = 0.70;	// probability that offspring inherit an allele from elite parent
	const unsigned K = 3;		// number of independent populations
	const unsigned MAXT = 2;	// number of threads for parallel decoding
	
	SampleDecoder decoder;			// initialize the decoder
	
	//READ DATA
	readData();

 	
	const long unsigned rngSeed = 0;	// seed to the random number generator
	MTRand rng(rngSeed);				// initialize the random number generator
	
	// initialize the BRKGA-based heuristic
	BRKGA< SampleDecoder, MTRand > algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);
	
	unsigned generation = 0;		// current generation
	const unsigned X_INTVL = 100;	// exchange best individuals at every 100 generations
	const unsigned X_NUMBER = 2;	// exchange top 2 best
	const unsigned MAX_GENS = 1000;	// run for 1000 gens

	do {
		algorithm.evolve();	// evolve the population for one generation
		
		if((++generation) % X_INTVL == 0) {
			algorithm.exchangeElite(X_NUMBER);	// exchange top individuals
		}
	} while (generation < MAX_GENS);


	std::cout << "Best solution found has objective value = "
	 		<< algorithm.getBestFitness() << std::endl;


	return 0;
}
