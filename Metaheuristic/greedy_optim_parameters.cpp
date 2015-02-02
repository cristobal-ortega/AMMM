/* Implementation of a vns-ts Metaheuristic */
/* for the Memory Allocation Problem		*/
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include <list>
#include <limits>
#include <algorithm>

using namespace std;

#define NREPET 20
#define NTMAX 10	 // 50
#define NUMITER 5000 //
#define KMAX 1
#define AVALUE 5
#define RANDOMMEMEX 0

/*
	INPUT FILE FORMAT:
		penalty 		# External memory access penalty (multiplier)
		n				# Number of data structures
		< n sizes >		# Size in memory for each data struct.
		< n costs >		# Access cost for each data struct.
		m				# Number of banks of memory
		< m capacities ># Capacity of each bank of memory
		o				# Number of conflicts
		< o structs A >	# Conflict a op b w/ cost d = {a b d}
		< o structs B >
		< o costs D >
*/
float penalty;
int n;
int m;
int o;

vector<int> A;
vector<int> B;
vector<int> D;

vector<int> S;
vector<int> E;
vector<int> C;

float OptimalSolution;

void printSHIT(vector< vector<int> > &X, float f)
{
	cout << "Total cost:" << f << endl;
	for(int i = 0; i < n; i++){
		for(int j = 0; j < m + 1; j++)
			cout << X[i][j] << " ";
		cout << endl;
	}
}


vector<int> get_used_capacity(vector< vector<int> > &X)
{
	vector<int> u (m,0);

	for(int i = 0; i < n; i++)
		for(int j = 0; j < m; j++)
			if (X[i][j])
				u[j] += S[i];

	return u;
}

void capacity_check(vector <int> u)
{
	for(int j = 0; j < m; j++)
	{
		if(u[j] > C[j])
		{
			cerr << "ALARM: Capacity exceeded for " << j << endl;
			exit(0);
		}
	}
}



/* Returns in which memory bank the data struct i is stored
   Returns -1 in none found */
int find_memory_bank(int i, vector< vector<int> > & X)
{
	for (int j = 0; j < (m + 1); j++)
	{
		if (X[i][j])
			return j;
	}
	return -1;
}

float compute_optimal_solution()
{
	float total = 0;
	// suma todas las E[i]
	for(int i = 0; i < n; i++)
	{
		total += E[i];
	}

	return total;
}

// Returns the state of the conflicts based on Matrix X
vector<float> get_conflict_status_Y(vector< vector<int> > & X)
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

float computeCostIncremental(vector<float> &Y, vector<float> & Yprima, int i, int j)
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

vector< vector<int> > greedyMemex()
{
	vector< vector<int> > X(n,vector<int>(m + 1));
	vector<float> Y(o,0);
	vector<float> Yprima(o,0);
	vector<int> u(m,0);

 	float f = 0;	// Solution cost
	float g = 0;
	int b = -1;
	int j;
	float h; 	//get max int

	for(int i = 0; i < n; ++i)
	{

		h = numeric_limits<float>::max();

		for(int j = 0; j < m + 1; ++j)
		{
			if( ((j == m) || ( u[j] + S[i] <= C[j])) )
			{
				vector< vector<int> > Xprima(X);
				Xprima[i][j] = 1;

				Y = get_conflict_status_Y(X);
				Yprima = get_conflict_status_Y(Xprima);

				// Additional cost if we place i in j
				g = computeCostIncremental(Y, Yprima, i, j);

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
	return X;
}

/* Obtains a random initial solution 					*/
/* This function returns a random feasible solution. 	*/
/* In each iteration we assign a data structure to a	*/
/* random (2) memory bank provided that capacity 		*/
/* constraints are satisfied (1).						*/
/* OUTPUT 												*/
/* dvar X	if struct i in mem. bank j -> X[i][j] = 1	*/
/* f		accumulates the cost of the data structs 	*/
/*			allocated									*/
/* OTHER 												*/
/* u		amount of mem capacity used of each bank	*/
/* f 		output var*/
vector< vector<int> > randomMemex()
{
	vector< vector<int> > X(n,vector<int>(m + 1));
	vector<float> Y(o,0);
	vector<float> Yprima(o,0);
	vector<int> u(m,0);

	float f = 0;		// Solution cost
	float g = 0;
	int j;


	for(int i = 0; i < n; ++i)
	{
		//cout << "Placing structure: " << i << endl;
		do
		{
			j = rand()%(m + 1);
			//cout << "\tTrying bank: " << j << endl;
		} while( !((j == m) || ( u[j] + S[i] <= C[j])) );

		vector< vector<int> > Xprima(X);

		Xprima[i][j] = 1;

		if (j < m)
			u[j] += S[i];
		Y = get_conflict_status_Y(X);
		Yprima = get_conflict_status_Y(Xprima);

		g = computeCostIncremental(Y, Yprima, i, j);

		X = Xprima;
		f += g;
	}
}

float solutionFunction(vector< vector<int> > & X)
{
	vector<float> Y = get_conflict_status_Y(X);
	float total = 0;
	// coste de conflictos
	for(int k = 0; k < o; k++)
	{
		total += Y[k]*D[k];
	}

	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j <= m; j++)
		{
			// HACES EL DOBLE DE MULTIPLICACIONES
			if (j < m)
				total += X[i][j] * E[i];
			else
 				total += X[i][j] * penalty * E[i];
		}
	}

	return total;
}

// True - If i found the pair <i,j> in tabulist
// False - Otherwise
bool find_in_tabu(list< pair<int,int> > &tabulist, int i, int j)
{
	std::list< pair<int,int> >::iterator findIter =
		std::find(tabulist.begin(), tabulist.end(), make_pair(i,j));
/*
	if (findIter != tabulist.end())
		cout << "pair <i,j> = " << i << "," << j << " FOUND!" << endl;
	else
		cout << "pair <i,j> = " << i << "," << j << " FOUND!" << endl;
*/
	return (findIter != tabulist.end());
}

vector< vector<int> > exploreNeighborhoodN0(vector< vector<int> > &X,
	vector<int> &u, list< pair<int,int> > &tabulist,
	int &itabu, int &htabu, int &jtabu, float &f)
{
	f = numeric_limits<float>::max();
	vector< vector<int> > Xprima;
	float fprima = 0;
	int b = -1;
	int a = -1;
	int c = -1;

	// Check for each data struct i, if theres a memory bank j that gives less cost
	for(int i = 0; i < n; i++)
	{
		int h = find_memory_bank(i, X);

		for(int j = 0; j < m + 1; j++)
		{
			// If the move is allowed (non-tabu etc) calculate the cost
			if((h != j) && (j == m || (S[i] + u[j]) <= C[j]) && !find_in_tabu(tabulist, i,j))
			{
				X[i][j] = 1;
				X[i][h] = 0;
				fprima = solutionFunction(X);

				if (fprima < f)
				{
					f = fprima;
					b = j;
					a = i;
					c = h;
				}

				X[i][j] = 0;
				X[i][h] = 1;
			}
		}
	}

	// Build new solution
	// Create Xprima and apply the min cost move
	// Set i-j-h-tabu with the (i,h,j) values of the min cost move
	Xprima = X;

	if (!(a == -1 || b == -1))
	{
		Xprima[a][b] = 1;
		Xprima[a][c] = 0;

		itabu = a;
		jtabu = b;
		htabu = c;

		// u[j] <- u[j] + S [i]
		u[jtabu] = u[jtabu] + S[itabu];
		// u[h] <- u[h] - S [i]
		u[htabu] = u[htabu] - S[itabu];


		//capacity_check(get_used_capacity(X));
	}
	else
		cerr << "N0: Couldn't find a min cost move." << endl;

	return Xprima;
}

vector< vector<int> > exploreNeighborhoodN1( vector< vector<int> > &X,
	vector<int> &u, list< pair<int,int> > &tabulist,
	int &itabu, int &htabu, int &jtabu, float &f)
{
	//Find non tabu min cost move (i,h,j), such that h != j and u[j] + s[i] <= c[j]
	f = numeric_limits<float>::max();
	vector< vector<int> > Xprima(n, vector<int>	(m + 1)); 						// Temporary solution applying a non tabu move
	vector< vector<int> > Xprimaprima(n, vector<int>(m + 1)); 					// Temporary solution if we need to fix the move
	float fprima;

	int btabu, ltabu;
	btabu = ltabu = itabu = htabu = jtabu = -1;

	// Check for each data struct i, if theres a memory bank j that gives less cost
	for(int i = 0; i < n; i++)
	{
		// Where is this Struct placed?
		int h = find_memory_bank(i, X);

		for(int j = 0; j < m + 1; j++)
		{
			// If the move is allowed (non-tabu) calculate the cost
			if((h != j) && (! find_in_tabu(tabulist, i,j)))
			{
				X[i][j] = 1;
				X[i][h] = 0;
				fprima = solutionFunction(Xprima);

				// Keep the min cost move so far.
				if (fprima < f)
				{
					f = fprima;
					itabu = i;
					htabu = h;
					jtabu = j;
				}

				X[i][j] = 0;
				X[i][h] = 1;
			}
		}
	}

	//cout << "Min cost move found..." << endl;
	// Build new solution
	// Create Xprima and apply the min cost move
	// Set i-j-h-tabu with the (i,h,j) values of the min cost move
	Xprima = X;
	//printSHIT(Xprima,fprima);
	if (!(itabu == -1 || jtabu == -1))
	{
		Xprima[itabu][jtabu] = 1;
		Xprima[itabu][htabu] = 0;

		if (jtabu < m)
			u[jtabu] = u[jtabu] + S[itabu];

		if (htabu < m)
			u[htabu] = u[htabu] - S[itabu];
	}

	//cout << htabu << " " << jtabu << " " << itabu << " " << endl;
	//cout << "New solution (un)feasible built..." << endl;
	// At this point is possible we have a solution exceeding the max capacity
	// of memory bank jtabu, we need to repair it.

	//cout << htabu << " " << jtabu << " " << itabu  << " " << endl;

	f = numeric_limits<float>::max();

	while ((jtabu < m) && u[jtabu] > C[jtabu])
	{
		f = numeric_limits<float>::max();
		//
		// With data struct l
		for(int l = 0; l < n; l++)
		{
			// We cannot move the data structure we just moved
			if(l != itabu)
			{
				// We are only interested in structs placed in jtabu
				if(find_memory_bank(l, Xprima) == jtabu)
				{
					//cout << "trying moving  " << l << " from " << jtabu << endl;
					for(int b = 0; b < (m+1); b++)
					{

						// Now we have to enforce the capacity constraint
						if((b != jtabu) && (b == m || (u[b] + S[l]) <= C[b])  && !find_in_tabu(tabulist, l, b))
						{
							//u = get_used_capacity(Xprima);
							Xprima[l][b] = 1;
							Xprima[l][jtabu] = 0;

							//cout << htabu << " " << jtabu << " " << itabu << " data " << l << " trying to put it in " << b << endl;
							fprima = solutionFunction(Xprima);

							if (fprima < f)
							{
								f = fprima;
								ltabu = l;
								btabu = b;
							}

							Xprima[l][b] = 0;
							Xprima[l][jtabu] = 1;
						}
					}

					//cout << htabu << " " << jtabu << " " << btabu << " " << ltabu << " " << endl;
					// Apply min cost move
				}
			}
		}

		if (!(ltabu == -1 || jtabu == -1))
		{
			//cout << "Relieving mem usage... " << endl;
			Xprima[ltabu][jtabu] = 0;
			Xprima[ltabu][btabu] = 1;

			//cout << "Moving to " << btabu << " "<< u[btabu] << endl;

			// Update used capacity
			if (btabu < m)
				u[btabu] += S[ltabu];

			if (jtabu < m)
				u[jtabu] -= S[ltabu];

			//cout << "Now " << btabu << " has "<< u[btabu] << endl;
		}
		else
			cerr << "Couldn't relieve memory. Maybe stuck. " << endl;

		//cout << u[jtabu] << " of "<< jtabu << " " << C[jtabu] << " used " << endl;
		ltabu = btabu = -1;
	}
	//cout << " Finish relieving.." << endl;

	//capacity_check(u);
	return Xprima;
}

vector< vector<int> > tabuMemex(vector< vector<int> > & X, int k, float &f_solution)
{
	// Init used capacity status
	vector<int> u(m,0);

	// Xsolution is the best solution found when stopping condition is met
	vector< vector<int> > Xsolution(n,vector<int>(m + 1));

	// Xprima is the solution from Exploring the Neighborhood
	vector< vector<int> > Xprima(n,vector<int>(m + 1));

	// Get the size of the tabulist (NT) like this NT = a * NTMAX * t0
	float t = rand()%2;
	int NT = AVALUE + NTMAX * t;
	float faux = numeric_limits<float>::max();
	int iter, a;
	f_solution = numeric_limits<float>::max();

	// Initialize capacity based on current solution X
	list< pair<int, int> > tabulist;

	iter = 0;
	while( iter < NUMITER and faux > OptimalSolution) //and solutionFunction(A, B, D, E, X) > 0)
	{

		int i, h, j;
		u = get_used_capacity(X);

		if (k)
		{
			Xprima = exploreNeighborhoodN1(X, u, tabulist , i, h, j, faux);
		}
		else
		{
			Xprima = exploreNeighborhoodN0(X, u, tabulist , i, h, j, faux);
		}

		// We take it as a working solution, even if its worse.
		X = Xprima;

		// Keep it as a final solution 'Xsolution' if its better than current.
		if(faux < f_solution)
		{
			//cout << " Exploring the neighborhood " << k << " I've found a better solution!" << endl;
			f_solution = faux;
			Xsolution = Xprima;
		}

		// Add i,j and i,h to the tabu list
		// check si está llena
		if (tabulist.size() == NT)
			tabulist.pop_front();
		tabulist.push_back(make_pair(i,h));

		if (tabulist.size() == NT)
			tabulist.pop_front();
		tabulist.push_back(make_pair(i,j));

		// Update the size of tabu list NT (TabuCol optimization)
		// The replacement
		if (((iter + 1) % NTMAX) == 0)
		{
			t = rand()%2;
			NT = AVALUE + NTMAX*t;

			int excess = tabulist.size() - NT;

			// If the new size is smaller than the num of elements in the list
			// we delete the first ones. FIFO
			for(int z = 0; z < tabulist.size(); z++)
				tabulist.pop_front();
		}
		iter++;
	}

	return Xsolution;
}

vector< vector<int> > destroy60pAtRandom(vector< vector<int> > &Xaux)
{
	vector< vector<int> > Xdestroyed = Xaux;
	//srand(time(NULL));
	int maxvictims = n * 0.6f;
	int nv = 0;

	while(nv < maxvictims)
	{
		// obtenemos una struct random
		int v = rand()%(n);
		int j;

		for(j = 0; j < m+1; j++)
		{
			if(Xdestroyed[v][j])
				break;
		}

		if(j < m+1)
		{
			//cout << "pam! " <<  v << " " << j << endl;
			Xdestroyed[v][j] = 0;
			nv++;
		}
	}
	return Xdestroyed;
}

vector< vector<int> > reconstructGreedy(vector< vector<int> > Xsolution)
{
	vector< vector<int> > X = Xsolution;
	vector<float> Y(o,0);
	vector<float> Yprima(o,0);
	vector<int> u = get_used_capacity(X);

	float f = 0;	// Solution cost
	float g = 0;
	int b = -1;
	float h;	//Best result so far.

	for(int i = 0; i < n; ++i)
	{

		// THIS IS WHAT WE CHANGE FROM GREEDY MEMEX
		// if the data struct is already assigned jump to the next;
		if (find_memory_bank(i, X) == -1)
		{
			Y = get_conflict_status_Y(X);

			//cout << "data " << i << " is not assigned" << endl;
			h = numeric_limits<float>::max();

			// Get the cost of adding the struct to mem bank j
			for(int j = 0; j < m + 1; ++j)
			{
				if( ((j == m) || ( (u[j] + S[i]) <= C[j])) )
				{
					//vector< vector<int> > Xprima(X);
					X[i][j] = 1;
					Yprima = get_conflict_status_Y(X);

					// Additional cost if we place i in j
					g = computeCostIncremental(Y, Yprima, i, j);

					// Keep this assignation in  if improves something
					if (g < h)
					{
						b = j;
						h = g;
					}
					X[i][j] = 0;
				}

			}

			X[i][b] = 1;

			//cout << b << " " << u[b] << " " << S[i];
			// updato
			if (b < m)
				u[b] += S[i];

			//capacity_check(get_used_capacity(Xsolution));

			f += h;

		}
	}
	//cout << "al final del reconstruct... "<< endl;
	//capacity_check(get_used_capacity(Xsolution));
	return X;
}

/* C = Capacities 	*/
/* S = sizes of each data structure 	*/
/* E = Access cost of each data struct 	*/
/* D = Conflicts found in the program   */
int main(int argc, char *argv[])
{
	float fprima, fsolution;
	vector< vector<int> > Xsolution(n,vector<int>(m + 1));
	vector< vector<int> > Xprima(n,vector<int>(m + 1));
	vector< vector<int> > X(n,vector<int>(m + 1));
	srand (time(NULL));

	/* read input with ... */
	cin >> penalty;
	/* ... data struct info */
	cin >> n;
	S = vector<int>(n);
	for(int i = 0; i < n; i++)
		cin >> S[i];

	E = vector<int>(n);
	for(int i = 0; i < n; i++)
		cin >> E[i];

	/* memory bank info */
	cin >> m;
	C = vector<int>(m);
	for(int j = 0; j < m; j++)
		cin >> C[j];

	cin >> o;

	A = vector<int>(o);
	for(int k = 0; k < o; k++)
		cin >> A[k];

	B = vector<int>(o);
	for(int k = 0; k < o; k++)
		cin >> B[k];

	D = vector<int>(o);
	for(int k = 0; k < o; k++)
		cin >> D[k];

	//cout << "Generating init solution...";

	OptimalSolution = compute_optimal_solution();
	// GENERAR A que es básicamente girar un vector que indica el orden d las estructuras
	if(RANDOMMEMEX)
	{
		Xsolution = randomMemex();
		//fsolution = solutionFunction(A,B,D,E,Xsolution);
	}
	else
	{
		Xsolution = greedyMemex();
		fsolution = solutionFunction(Xsolution);
	}

	//cout << " OK!" << endl;

	//cout << "Optimal solution : " << OptimalSolution << endl;
	// k to select neighborhood [0/1]
	int k = 0;
	int i = 0;

	//cout << "\n#Iterative phase - cost so far: " << fsolution << endl;
	// After NREPET iterations without improving the solution, the program will end

	while (i < NREPET)
	{
		cerr << " Iteration " << i << endl;
		X = destroy60pAtRandom(Xsolution);
		X = reconstructGreedy(X);

		//capacity_check(get_used_capacity(X));
		//cout << "double ok!" << endl;

		// Local search - Tabu Search
		Xprima = tabuMemex(X, k, fprima);

		// If a better solution is found, keep it.
		// Also if the solution is improved reset i counter.
		if (fprima < fsolution)
		{
			cerr << "Local tabu search improved the solution! " << fprima << endl;
			Xsolution = Xprima;
			fsolution = fprima;
			i = 0;
			k = 0;
		}
		else
		{
			// Cycle the value of k: 0->1->0->1...
			k = (k == KMAX)? 0 : k + 1;
			++i;
		}
	}

	//printSHIT(Xsolution,fsolution);
	cout << fsolution << endl;
}
