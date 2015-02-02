/*********************************************
 * OPL 12.6.0.0 Model
 * Author: bscuser
 * Creation Date: 26/11/2014 at 11:21:42
 *********************************************/
int Nn=...;
int Nm=...;
int No=...;
float p=...;

int d=1;
int a=2;
int b=3;
 
range N=1..Nn;
range M=1..Nm;
range O=1..No;

int s[i in N]=...;
int c[j in M]=...;
int e[i in N]=...;
int conf[k in O][1..3]=...;


dvar boolean x[i in N][j in 1..Nm+1];
dvar float y[k in O];

// Objective
minimize sum(k in O)(y[k]*conf[k][d])+sum(i in N, j in M)(e[i]*x[i][j])+p*sum(i in N)(e[i]*x[i][Nm+1]);
subject to{
// Constraint 1
forall(i in N)
  sum(j in 1..Nm+1)x[i][j] == 1;
// Constraint 2
forall(j in M)
  sum(i in N)(x[i][j]*s[i]) <= c[j];
// Constraint 3
forall(j in M,k in O)
  x[conf[k][a]][j]+x[conf[k][b]][j] <= 1 + y[k];
// Constraint 4
forall(j in M,k in O)
  x[conf[k][a]][j]+x[conf[k][b]][Nm+1] <= 1 + (1/p)*y[k];
//Constraint 5
forall(j in M,k in O)
  x[conf[k][a]][Nm+1]+x[conf[k][b]][j] <= 1 + (1/p)*y[k];
//Constraint 6
forall(k in O)
  x[conf[k][a]][Nm+1]+x[conf[k][b]][Nm+1] <= 1 + (1/(2*p))*y[k];
//Constraint 7
forall(k in O)
  y[k] >= 0;
}

