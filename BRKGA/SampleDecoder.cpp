/*
 * SampleDecoder.cpp
 *
 *  Created on: Jan 14, 2011
 *      Author: rtoso
 */

#include "SampleDecoder.h"
#include <iostream>
#include <sys/time.h>

using namespace std;

SampleDecoder::SampleDecoder() { }

SampleDecoder::~SampleDecoder() { }



extern float penalty;
extern int n;		//# of structs
extern int m;		//# of banks
extern int o;		//# of conflicts

/* C = Capacities 	*/
/* S = sizes of each data structure 	*/
/* E = Access cost of each data struct 	*/
/* D = Conflicts found in the program   */

extern int *C_orig;
extern int *S_orig;
extern int *E_orig;
extern int *A_orig;
extern int *B_orig;
extern int *D_orig;


void printSHIT(vector< vector<int> > & X, float f)
{
	cout << "Such Statistics: " << endl;
	cout << "Total cost:" << f << endl;
	cout << "Data Structures per banks:" << endl;
	for(int i = 0; i < n; i++){
		for(int j = 0; j < m + 1; j++)
			cout << X[i][j] << " ";
		cout << endl;
	}

	cout << "Kisses" << endl;
}

void checkRead(){
	cout << "CHECKING READ" << endl;
	cout << penalty << endl;
	/* ... data struct info */
	cout << n<< endl;

	for(int i = 0; i < n; i++)
		cout << S_orig[i] << " ";

	cout << endl;
	

	for(int i = 0; i < n; i++)
		cout << E_orig[i] << " ";

	cout << endl;

	/* memory bank info */
	cout << m<< endl;

	for(int j = 0; j < m; j++)
		cout << C_orig[j] << " ";

	/* 	conflicts info
		A[0..o-1]
		B[0..o-1]
		D[0..o-1]
	*/
	cout << o<< endl;

	for(int k = 0; k < o; k++)
		cout << A_orig[k] << " ";
	cout << endl;
	for(int k = 0; k < o; k++)
		cout << B_orig[k] << " ";
	cout <<endl;
	for(int k = 0; k < o; k++)
		cout << D_orig[k] << " ";
	cout << endl;
}


int find_memory_bank(int i, vector< vector<int> > & X)
{
	//cout << n << " " << m << endl;
	for (int j = 0; j < m + 1; j++)
	{
		//cout << i << " " << j << endl;
		if (X[i][j])
			return j;
	}

	return -1;
}

vector<float> get_conflict_status_Y (
	vector<int> &A, vector<int> &B , vector< vector<int> > & X)
{
	vector<float> Y(o,0);
	int bank_a, bank_b;
	int ext_memory = m;

	// Update every conflict
	for(int k = 0; k < o; k++)
	{
		bank_a = find_memory_bank(A[k], X);
		bank_b = find_memory_bank(B[k], X);

		if(bank_a == ext_memory || bank_b == ext_memory)
		{
			/* one in ext_memory = p, both = 2p */
			Y[k] = (bank_a == ext_memory) + (bank_b == ext_memory);
			Y[k] *= penalty;
		}
		else
		{
			if (bank_a == -1 || bank_b == -1) //
			{
				/* if one of the structs not allocated cost = 0 */
				Y[k] = 0;
			}
			else if(bank_a == bank_b)
			{
				/* both in the same bank */
				Y[k] = 1;
			}
			else
			{
				/* different banks */
				Y[k] = 0;
			}
		}
	}

	return Y;
}


float computeCostIncremental(vector<float> &Y, vector<float> & Yprima,
	vector<int> &A, vector<int> &B, int *D, vector<int> &E, int i, int j)
{
	float base_cost = 0;
	float conflict_cost_prima = 0;
	float conflict_cost = 0;

	base_cost = (j < m)? E[i] : penalty*E[i];

	/* 	Examine all conflicts.
		The ones affected by i have its cost (Dk) mult.
		by the status [0,1,p,2p]
	*/

	for( int k = 0; k < o; ++k)
	{
		if( A[k] == i || B[k] == i)
		{
			conflict_cost += Y[k]*D[k];
			conflict_cost_prima += Yprima[k]*D[k];
		}
	}

	return base_cost + (conflict_cost_prima - conflict_cost);
}

float greedyMemex(vector<int> & S, vector<int> & E, int *C,
	vector<int> & A ,vector<int> & B, int * D)
{
	vector< vector<int> > X(n,vector<int>(m + 1));
	vector<float> Y(o,0);
	vector<float> Yprima(o,0);
	vector<int> u(m,0);

 	float f = 0;	// Solution cost
	float g = 0;
	int b = -1;
	int j;
	int h; 	//get max int
	for(int i = 0; i < n; ++i)
	{
		h = 99999999;

		for(int j = 0; j < m + 1; ++j)
		{
			if( ((j == m) || ( u[j] + S[i] <= C[j])) )
			{
				vector< vector<int> > Xprima(X);
				Xprima[i][j] = 1;
				Y = get_conflict_status_Y(A, B, X);
				Yprima = get_conflict_status_Y(A, B, Xprima);
				// Additional cost if we place i in j
				g = computeCostIncremental(Y, Yprima, A, B, D, E, i, j);
				// Keep this assignation if improves something
				if (g < h)
				{
					b = j;
					h = g;
				}
			}

		}

		X[i][b] = 1;

		if (b < m)
			u[b] += S[i];

		f += h;

	}
	//printSHIT(X,f);
	return f;
}



/*
Algorithm for decoder:


s = chromosome[i] * structs[i].size
s = sort(chromosome, DESC)

S before:
 1     2             n
-------------------------
2.3 | 24.1 | ... | 22.2 |
-------------------------

S after:
  2     n            1
-------------------------
24.1 | 22.2 | ... | 2.3 |
-------------------------

for each s in S:
	m = bankLowestCost( estado_actual, conflictos, s)
	update banks (M)
	update solution

return f(solution)	
*/

/* n = # of structs
/* m = # of banks
/* o = # of conflicts

/* C = Capacities 	*/
/* S = sizes of each data structure 	*/
/* E = Access cost of each data struct 	*/
/* D = Conflicts found in the program   */

struct ordering {
  double value;
  int size;
  int cost;
  int old_position;
} ;


bool compareOrdering( ordering a, ordering b){
	return a.value > b.value;

}


// Runs in \Theta(n \log n):
double SampleDecoder::decode(const std::vector< double >& chromosome) const {

	vector<int> S_r(n);
	vector<int> E_r(n);

	vector<ordering> K(n);
	vector<int>A_r(o);
	vector<int>B_r(o);
	vector<int>D_r(o);
	vector<int> C_r(m);

	#pragma omp parallel 
	{
		
		#pragma omp for
		for(int i = 0; i < n; ++i){
			K[i].value = chromosome[i] * S_orig[i];
			K[i].size = S_orig[i];
			K[i].cost = E_orig[i];		
			K[i].old_position = i;
		}	
		
		#pragma omp single
		std::sort (K.begin(), K.end(), compareOrdering);

		//for(int i = 0; i < m; ++i)
			//C_r[i] = C_orig[i];

		#pragma omp for
		for(int i = 0; i < n; ++i){
		
			S_r[ K[i].old_position ] = K[i].size;
			E_r[ K[i].old_position ] = K[i].cost;
		}



		#pragma omp for
		for(int i = 0; i < o; ++i){
			int old_position_a = A_orig[i];
			int old_position_b = B_orig[i];
			for(int j = 0; j < n; ++j){
				if( K[j].old_position == old_position_a )
					A_r[i] = j;
				if( K[j].old_position == old_position_b )
					B_r[i] = j;
			}
		}
	}

	float value = greedyMemex(S_r, E_r, C_orig, A_r, B_r, D_orig);

	

	//std::cout << "Size of chromosome: " << chromosome.size() << std::endl;
	//checkRead();
	
	// sample fitness is the first allele
	//return chromosome.front();

	return value;
}
